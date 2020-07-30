//
// Created by user on 22.08.2019.
//

#ifndef TESTAPP_PROPERTYCLASS_H
#define TESTAPP_PROPERTYCLASS_H

#include <functional>
#include <map>
#include <vector>
#include <memory>
#include <sigslot/signal.hpp>
#include "Variable.h"
#include "Types.h"

/**
 * Класс хранит всю информацию о имеющихся property
 */

class TPropertyClass;

using TPtrPropertyClass = std::shared_ptr<TPropertyClass>;
using TWPtrPropertyClass = std::weak_ptr<TPropertyClass>;
using TGetFun = std::function<TVariable(const TPropertyClass *)>;
using TSetFun = std::function<void(TPropertyClass *, const TVariable &)>;
using TGetIndFun = std::function<TVariable(const TPropertyClass *, int)>;
using TChangePropertyClass = sigslot::signal<>;
using TIdConnect = sigslot::scoped_connection;
using TChangeThePropertyClass = std::function<void(TPtrPropertyClass value, const TString& fullName)>;

#include "PropertyClass.hpp"


struct TPropInfo{
private:
    TString name;
    TString type;
    TGetFun get;//содержит либо геттер свойства, либо геттер количества элементов
    TSetFun set;//содержит либо сеттер свойства, либо функцию добавления в массив свойство

    TSetFun delArray;
    TGetIndFun getArray;

    bool isStoring = true;
    bool isLoading = true;

    bool isClass = false;
    bool isCheckName = false;
public:
    TPropInfo() = default;

    TPropInfo(const TString &_name, const TString &_type, bool _isClass) : name(_name), type(_type), isClass(_isClass) {}

    inline TString Name() const { return name; };

    inline TString Type() const { return type; }

    //для обычных свойств
    TPropInfo &Get(TGetFun &&fun)
    {
        get = fun;
        return *this;
    }

    TPropInfo &Set(TSetFun &&fun)
    {
        set = fun;
        return *this;
    }

    //для индексированных свойств
    TPropInfo &GetArray(TGetFun &&countArray, TGetIndFun &&_getArray)
    {
        get = countArray;
        getArray = _getArray;
        return *this;
    }

    TPropInfo &AddArray(TSetFun &&fun)
    {
        set = fun;
        return *this;
    }

    TPropInfo &DelArray(TSetFun &&fun)
    {
        delArray = fun;
        return *this;
    }

    //вызовы для обычных свойств
    TVariable CallGet(const TPropertyClass *obj) const { return get(obj); };

    void CallSet(TPropertyClass *obj, const TVariable &value) const { set(obj, value); };

    //вызовы для индекс свойств
    TVariable CallGetArray(const TPropertyClass *obj, int index) const { return getArray(obj, index); }

    int CallGetCountArray(const TPropertyClass *obj) const { return int(get(obj).ToInt()); }

    void CallAddArray(TPropertyClass *obj, const TVariable &value) const { set(obj, value); }

    void CallDelArray(TPropertyClass *obj, const TVariable &value) const { delArray(obj, value); }

    TPropInfo &SetStorable(bool value)
    {
        isStoring = value;
        return *this;
    }

    TPropInfo &SetLoadable(bool value)
    {
        isLoading = value;
        return *this;
    }

    TPropInfo &NoSerialization()
    {
        isLoading = isStoring = false;
        return *this;
    }

    TPropInfo &SetCheckName(bool value)
    {
        isCheckName = value;
        return *this;
    }


    inline bool IsValid() const { return name.size() && get; }

    inline bool IsReadOnly() const { return static_cast<bool>(set) == false; }

    inline bool IsPod() const { return !isClass; }
    inline bool IsClass() const { return isClass && !IsArray(); }
    inline bool IsArray() const { return static_cast<bool>(getArray); }

    inline bool IsStorable(bool always = false) const { return IsValid() && ( always || isStoring); }
    inline bool IsLoadable(bool always = false) const { return IsValid() && set && ( always || isLoading); }
    inline bool IsReadLoadable(bool always = false) const { return IsValid() && ( always || isLoading); }

    inline bool IsCheckName() const { return isCheckName; }
};

class TPropertyManager
{
public:
    using TFunCreate = std::function<TPtrPropertyClass()>;

    static size_t CountManagers();
    static const TPropertyManager &Manager(const TString &type);

    TPropertyManager() = default;
    TPropertyManager(const TString &_type, TFunCreate &&_createType);

    TString Type() const;

    TFunCreate CreateTypeFun() const;

    TPtrPropertyClass CreateObj() const;

    inline bool IsValid() const { return type.size(); }

    TPropInfo &AddProperty(const TString &type, const TString &name, bool isClass = false);
    void AppendProperties(TPropertyManager &value);

    const TPropInfo &Property(int index) const;
    const TPropInfo &FindProperty(const TString &nameProperty) const;
    int IndexProperty(const TString &nameProperty) const;
    size_t CountProperty() const;

    TVariable ReadProperty(int index, const TPropertyClass *obj) const;
    void WriteProperty(int index, TPropertyClass *obj, const TVariable &value);

    int ReadCountInArray(int index, const TPropertyClass *obj) const;
    TVariable ReadFromArray(int index, const TPropertyClass *obj, int indexArray) const;

    void AddToArray(int index, TPropertyClass *obj, const TVariable &value);
    void DelFromArray(int index, TPropertyClass *obj, const TVariable &value);

    bool IsInit() const;

    void SetIsInit(bool value = true);
    using TVecPropManList = std::vector<TPropertyManager*>;
    void AddChildManager(TPropertyManager& value);
    TVecPropManList ListChildManagers() const;
    bool IsCustableTo(const TPropertyManager& value) const;//можно ли текущий менеджер привести к переданному(value is base class?)
private:
    using THashProperties = std::map<TString, size_t>;
    using TProperties = std::vector<TPropInfo>;
    using TManagers = std::map<TString, TPropertyManager *>;

    STATIC(TManagers, All)

    TPropertyManager* baseManager = nullptr;
    TVecPropManList childManagers;

    TString type;
    TFunCreate createTypeFun;

    THashProperties hashProperties;
    TProperties properties;
    bool isInit = false;

    void AddProperty(TPropInfo &&value);

    bool CheckIndex(int index) const;
    bool CheckGet(int index) const;
    bool CheckSet(int index) const;
};

#include <sigslot/signal.hpp>

class TPropertyClass: public std::enable_shared_from_this<TPropertyClass>{
protected:
    TString name;
    TChangePropertyClass change;
public:
    PROPERTIES_BASE(TPropertyClass)
    static bool InitProperties() noexcept;
    static TPtrPropertyClass CreateFromType(const TString &type);

    virtual ~TPropertyClass() = default;

    TString TypeClass() const;

    void SetName(const TString &value);
    TString Name() const;

    int IndexProperty(const TString &nameProperty) const;
    size_t CountProperty() const;

    TVariable ReadProperty(int index);
    TVariable ReadProperty(const TString &nameProperty);

    void WriteProperty(int index, const TVariable &value);
    void WriteProperty(const TString &nameProperty, const TVariable &value);

    int CountInArray(int index) const;
    int CountInArray(const TString &nameProperty) const;

    TVariable ReadFromArray(int index, int indexArray) const;
    TVariable ReadFromArray(const TString &nameProperty, int indexArray) const;

    void AddToArray(int index, const TVariable &value);
    void AddToArray(const TString &nameProperty, const TVariable &value);

    void DelFromArray(int index, const TVariable &value);
    void DelFromArray(const TString &nameProperty, const TVariable &value);

    void Change();//оповестить тех кто подписан что объект изменился
    TChangePropertyClass& OnChange();
};



class TCommunicClass{
private:
    TWPtrPropertyClass commun;
public:
    using TRegVector = std::vector<TWPtrPropertyClass>;
    using TFunCreate = std::function<void(const TString& name)>;
    using TRegForCreate = std::map<TString, TFunCreate>;
    void CommunReg(const TWPtrPropertyClass& value)
    {
        commun = value;
        Append(commun);
    }

    virtual ~TCommunicClass()
    {
        Remove(std::move(commun));
    }

    virtual TPtrPropertyClass FindInCommunic(const TString& value){ return TPtrPropertyClass(); }

    static TPtrPropertyClass FindCommun(const TString& name, bool autoCreate = false)
    {
        for(auto v : Commun())
        {
            TPtrPropertyClass ptr = v.lock();
            if (ptr && ptr->Name() == name) return ptr;
        }
        if(autoCreate)
        {
            CreateReg(name);
            return FindCommun(name, false);
        }
        return TPtrPropertyClass();
    }
    static void CreateReg(const TString& name)
    {
        auto it = ForCreate().find(name);
        if(it != ForCreate().end())
            it->second(name);
    }
    STATIC(TRegVector, Commun);
    STATIC(TRegForCreate, ForCreate);

    static void Append(const TWPtrPropertyClass& value)
    {
        Commun().emplace_back(value);
    }
    static void Remove(TWPtrPropertyClass&& value)
    {
        for(auto it = Commun().begin(); it != Commun().end(); it++)
            if(it->owner_before(value) == false && value.owner_before(*it) == false)
            {
                Commun().erase(it);
                break;
            }
    }
};

using TPtrCommunicClass = std::shared_ptr<TCommunicClass>;

template<typename T>
TVariable PropertyClassToVariable(const std::shared_ptr<T> &value)
{
    return TVariable(std::make_shared<TVariableExtValue<TPtrPropertyClass>>(value));
}

template<typename T>
TVariable PropertyClassToVariable(const std::shared_ptr<T> &&value)
{
    return TVariable(std::make_shared<TVariableExtValue<TPtrPropertyClass>>(value));
}

template <typename T>
T CreateFromType(const TString &type)
{
    return std::dynamic_pointer_cast<typename T::element_type>(TPropertyClass::CreateFromType(type));
}

inline TPtrPropertyClass VariableToPropClass(const TVariable &value)
{
    return value.ToType<TPtrPropertyClass>();
}

template<typename T>
T VariableCastTo(const TVariable &value)
{
    return VariableToPropertyClassImpl<T>(value);
}

inline TPtrPropertyClass SafePtrInterf(TPropertyClass* value)
{
    return TPtrPropertyClass{value, [](TPropertyClass*){/*No deleter*/}};
}

template <typename T>
    std::vector<T> ListProps(const TPtrPropertyClass& value, const TString& listProp, const TString& valProp)
    {
        //int intList = value->IndexProperty(listProp);

    }

TVecString ListNames(const TPtrPropertyClass& value, const TString& listProp);


#endif //TESTAPP_PROPERTYCLASS_H
