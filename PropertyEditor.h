//
// Created by user on 30.09.2019.
//

#ifndef TESTAPP_PROPERTYEDITOR_H
#define TESTAPP_PROPERTYEDITOR_H

#include <deque>
#include "PropertyClass.h"

class TObjTree;
class TCustClass;


using TPtrObjTree = std::shared_ptr<TObjTree>;
using TConstPtrObjTree = std::shared_ptr<const TObjTree>;
using TConstWPtrObjTree = std::weak_ptr<const TObjTree>;

class TObjTree : public std::enable_shared_from_this<TObjTree>{
public:
    TObjTree(const TConstWPtrObjTree& par = TPtrObjTree(), int indProp = -1);

    inline int IndProp() const { return indProp; };
    inline const TWPtrPropertyClass& Obj() const { return obj; };
    inline TPtrPropertyClass LockObj() const { return obj.lock(); };
    inline const TConstWPtrObjTree& Parent() const { return parent; };             //родительский TObjTree
    inline TConstPtrObjTree LockParent() const { return parent.lock(); };
    inline TObjTree* LockParentPtr() const { return (TObjTree*)parent.lock().get();}

    void Clear();//очищаем полностью объект
    virtual void SetObj(const TPtrPropertyClass& value);

    const TPtrObjTree& Item(size_t index) const;
    size_t CountItems(bool autoLoad = true) const;

    void RescanItems();

    void AddChild(TPtrPropertyClass value, int indProp);
    void DelChild(TPtrPropertyClass value, int indProp);

    void SetCustomClass(TCustClass* value);

    TString Name() const;
    TVariable Value(bool isType = true) const;
    void SetValue(const TVariable& value);

    bool HasChildren() const;//отображает может ли быть объекты в children

    bool IsLoaded() const;

    bool IsProp() const;  //отображает это свойство или класс
    bool IsCheckable() const;
    bool IsColor() const;
    bool IsBool() const;
    bool IsEditable() const;

    int Num(int def = -1) const;    //номер по порядку в Child списке родителя

    bool IsChecked() const;
    void SetIsChecked(bool value);

    using TVecObjTree = TPtrVector<TObjTree>;

    TCustClass* ClassCustoms(bool checkClass = true) const;
    TCustClass* PropCustoms() const;
    using TArrayInfo = std::tuple<TString, int>;
    using TVectArrayInfo = std::vector<TArrayInfo>;
    TVectArrayInfo ArrayInfo() const;//возвращает список свойств массивов и их номера
private:
    TConstWPtrObjTree parent;
    TCustClass* info = nullptr;
    TWPtrPropertyClass obj;

    int indProp = -1;

    void LoadItems() const;

    mutable std::vector<TPtrObjTree> items;
    mutable bool isLoaded = false;
    TIdConnect idChange;
    TIdConnect idDelete;

    bool HasProp(const TPtrPropertyClass& value) const;
    bool HasChild(const TPtrPropertyClass& value) const;

    TCustClass* RootInfo() const;
    TCustClass* ClassCustoms(const TPropertyManager &man, bool checkClass) const;

    bool IsProp(TPtrPropertyClass& lock) const;

    virtual void BeginDelete(TObjTree* objTree);
    virtual void EndDelete(TObjTree* objTree);
    virtual void BeginAdd(TObjTree* objTree);
    virtual void EndAdd(TObjTree* objTree);
};

enum class TShowProp{
    All,        //отображать все значения
    None,       //не отображать ничего
    Select,     //отображать или нет указано в свойстве, если не найдено то не отображать
    SelTrue,    //по умолчанию true
    Parent,      //отображать в зависимости от родительского элемента
    Function    //отображать или нет решает функция
};

enum class TShowClass{
    All,        //отображать все классы
    None,       //не отображать ничего
    SelType,    //отображать если есть в списке типов
    SelProp,    //отображать есть в списке типов и в списке свойств
    Parent      //отображать в зависимости от родительского элемента
};

using TCheckPropFun = std::function<bool(TPropertyClass*, const TString&)>;
using TComboListFun = std::function<std::vector<TString>()>;

struct TCustProp{
    bool visible = true;
    TString format;
    double min = NAN;
    double max = NAN;
    TComboListFun comboFun;
    TCustProp& SetVisible(bool value) { visible = value; return *this; }
    TCustProp& SetFormat(const TString& value) { format = value; return *this; }
    TCustProp& SetMin(double value) { min = value; return *this; }
    TCustProp& SetMax(double value) { max = value; return *this; }
    TCustProp& SetComboFun(const TComboListFun& value) { comboFun = value; return *this; }

};

class TCustClass{
    TShowClass showClasses = TShowClass::All;
    TShowProp showProperty = TShowProp::All;
    TShowProp editProperty = TShowProp::None;
    std::map<const TPropertyManager*, TCustClass> types;
    std::map<TString, TCustProp> props;
    TString valueClassProperty = "name";
    TCheckPropFun checkPropFun;
public:

    inline TShowClass ShowClasses() const { return showClasses; }
    inline TShowProp ShowProperty() const { return showProperty; }
    inline TShowProp EditProperty() const { return editProperty; }
    inline TString ValueClassProperty() const { return valueClassProperty; }

    inline size_t CountTypes() const { return types.size(); }
    inline size_t CountProps() const { return props.size(); }
    TCheckPropFun CheckPropFun() const { return checkPropFun; }

    inline TCustClass& SetShowClasses(TShowClass value)  { showClasses = value;  return *this; }
    inline TCustClass& SetShowProperty(TShowProp value) { showProperty = value; return *this; }
    inline TCustClass& SetEditProperty(TShowProp value) { editProperty = value; return *this; }
    inline TCustClass& SetValueClassProperty(const TString& value) { valueClassProperty = value; return *this; }
    inline TCustClass& SetCheckPropFun(const TCheckPropFun& value) { checkPropFun = value; return *this; }

    TCustClass& AddType(const TString& typeName, TShowProp sp = TShowProp::Select, TShowClass sc = TShowClass::Parent);
    TCustClass& AddType(const TString &typeName, const TCustClass& value);
    TCustProp& AddProp(const TString& propName, bool visible = true);
    TCustClass& AddProps(const TString& props);
    TCustClass& AddTypeProp(const TString& typeName, const TString& props);

    TCustClass* Info(const TString& propMan);
    TCustClass* Info(const TPropertyManager& propMan);
    bool CheckType(const TPropertyManager& value, const TString& nameProp);
    bool CheckProp(TPropertyClass* obj, const TString& value);

    void Clear();
    TCustProp& CustProp(const TString& value);
};

class TPropertyEditor{
public:
    TPropertyEditor(const TPtrObjTree& t = std::make_shared<TObjTree>());
    void Clear();
    virtual void SetObject(const TPtrPropertyClass& value);

    TWPtrPropertyClass Obj() const;
    TPtrPropertyClass LockObj() const;

    TObjTree& Tree();
    TCustClass& ClassCustoms();

    TPropertyEditor& SetIsAll(bool value = true);
    bool IsAll() const;

    TPropertyEditor& SetIsAllType(bool value = true);
    bool IsAllType() const;

    TPropertyEditor& SetIsAllProperty(bool value = true);
    bool IsAllProperty() const;

    TPropertyEditor& SetIsEdit(bool value = true);
    bool IsEdit() const;

protected:
    TCustClass classCustoms;
    TPtrObjTree tree;
    void SetTree(const TPtrObjTree& value);
};


#endif //TESTAPP_PROPERTYEDITOR_H
