//
// Created by user on 22.08.2019.
//

#ifndef TESTAPP_PROPERTYCLASS_H
#define TESTAPP_PROPERTYCLASS_H

#include <functional>
#include <map>
#include <vector>
#include <memory>
#include "Variable.h"
#include "Types.h"

/**
 * Класс хранит всю информацию о имеющихся property
 */

class TPropertyClass;

using TPtrPropertyClass = std::shared_ptr<TPropertyClass>;
using TGetFun = std::function<TVariable(const TPropertyClass *)>;
using TSetFun = std::function<void(TPropertyClass *, const TVariable &)>;
using TGetIndFun = std::function<TVariable(const TPropertyClass *, int)>;


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

    int64_t CallGetCountArray(const TPropertyClass *obj) const { return get(obj).ToInt(); }

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


    inline bool IsValid() const { return name.size() && get; }

    inline bool IsStorable() const { return IsValid() && isStoring && get; }
    inline bool IsLoadable() const { return IsValid() && isLoading && set; }

    inline bool IsReadOnly() const { return static_cast<bool>(set) == false; }

    inline bool IsClass() const { return isClass; }
    inline bool IsArray() const { return static_cast<bool>(getArray); }
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

    int64_t ReadCountInArray(int index, const TPropertyClass *obj) const;
    TVariable ReadFromArray(int index, const TPropertyClass *obj, int indexArray) const;

    void AddToArray(int index, TPropertyClass *obj, const TVariable &value);
    void DelFromArray(int index, TPropertyClass *obj, const TVariable &value);

    bool IsInit() const;

    void SetIsInit(bool value = true);
    TPropertyManager* BaseManager() const;
    TPropertyManager* ChildManager(int index) const;
    size_t CountChildManager() const;
    void AddChildManager(TPropertyManager* value);
private:
    using THashProperties = std::map<TString, size_t>;
    using TProperties = std::vector<TPropInfo>;
    using TManagers = std::map<TString, TPropertyManager *>;

    STATIC(TManagers, All)

    TPropertyManager* baseManager = nullptr;
    std::vector<TPropertyManager*> childManagers;

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

class TPropertyClass
{
protected:
    TString name;
public:
    PROPERTIES_BASE(TPropertyClass)
    static bool InitProperties();
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

    int64_t CountInArray(int index) const;
    int64_t CountInArray(const TString &nameProperty) const;

    TVariable ReadFromArray(int index, int indexArray) const;
    TVariable ReadFromArray(const TString &nameProperty, int indexArray) const;

    void AddToArray(int index, const TVariable &value);
    void AddToArray(const TString &nameProperty, const TVariable &value);

    void DelFromArray(int index, const TVariable &value);
    void DelFromArray(const TString &nameProperty, const TVariable &value);
};

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

#endif //TESTAPP_PROPERTYCLASS_H
