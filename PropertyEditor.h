//
// Created by user on 30.09.2019.
//

#ifndef TESTAPP_PROPERTYEDITOR_H
#define TESTAPP_PROPERTYEDITOR_H

#include <deque>
#include "PropertyClass.h"

class TObjTree;
class TCustClass;
using TFunUpdateTree = std::function<void(TObjTree* value)>;

class TObjTree{
public:
    TObjTree(const TPtrPropertyClass& value = TPtrPropertyClass(), TObjTree* parent = nullptr, int indProp = -1);
    void Clear();//очищаем полностью объект
    void ClearChildren();//очищаем объекты владения
    void SetInfo(TCustClass* value);
    void SetObj(const TPtrPropertyClass& value);
    const TPtrPropertyClass& Obj() const;
    int IndProp() const;

    void Load(bool refind = false);

    bool IsLoaded() const;
    bool IsChildren() const;//отображает может ли быть объекты в children
    bool IsProp() const;  //отображает это свойство или класс
    bool IsCheckable() const;
    bool IsColor() const;
    bool IsBool() const;
    bool IsEditable() const;

    size_t CountProps() const;
    TObjTree& Prop(int index);

    size_t CountChildren() const;
    TObjTree& Child(int index);
    const TObjTree& Child(int index) const;
    void AddChild(TPtrPropertyClass value, int indProp);
    void DelChild(TObjTree* value);
    void DelChild(TPtrPropertyClass value, int indProp);

    int LoadedCount();
    int LoadedCountAll();

    TString Name() const;
    TVariable Value(bool isType = true) const;
    void SetValue(const TVariable& value);

    TObjTree* Parent();             //родительский TObjTree
    int Num(int def = -1) const;    //номер по порядку в Child списке родителя

    using TArrayInfo = std::tuple<TString, int>;
    using TVectArrayInfo = std::vector<TArrayInfo>;
    TVectArrayInfo ArrayInfo() const;//возвращает список свойств массивов и их номера

    bool IsChecked() const;
    void SetIsChecked(bool value);
    void SetFunUpdateTree(TFunUpdateTree value);
    using TVecObjTree = TPtrVector<TObjTree>;

    TCustClass* ThisInfo() const;
private:
    TObjTree* parent = nullptr;
    TCustClass* info = nullptr;
    TFunUpdateTree update;
    TPtrPropertyClass obj;
    TIdConnect idChange;
    int indProp = -1;

    void CallUpdate();
    TFunUpdateTree GetFunUpdate();

    TVecObjTree props;
    TVecObjTree children;
    bool isLoaded = false;
    TChangeThePropertyClass funChecked;
    TChangeThePropertyClass FindFunChecked() const;

    bool HasChild(const TPtrPropertyClass& value) const;

    TCustClass* RootInfo() const;
    TCustClass* CustInfo(const TPropertyManager& man) const;
};

enum class TShowKind{
    All,        //отображать все значения
    None,       //не отображать ничего
    Select,     //отображать или нет указано в свойстве
    Parent,      //отображать в зависимости от родительского элемента
    Function    //отображать или нет решает функция
};

using TCheckPropFun = std::function<bool(TPropertyClass*, const TString&)>;

struct TCustProp{
    bool visible = true;
    TString format;
    double min = NAN;
    double max = NAN;
    TCustProp& SetVisible(bool value) { visible = value; return *this; }
    TCustProp& SetFormat(const TString& value) { format = value; return *this; }
    TCustProp& SetMin(double value) { min = value; return *this; }
    TCustProp& SetMax(double value) { max = value; return *this; }
};

class TCustClass{
    TShowKind showClasses = TShowKind::All;
    TShowKind showProperty = TShowKind::All;
    TShowKind editProperty = TShowKind::None;
    std::map<const TPropertyManager*, TCustClass> types;
    std::map<TString, TCustProp> props;
    TString valueClassProperty = "name";
    TCheckPropFun checkPropFun;
public:

    inline TShowKind ShowClasses() const { return showClasses; }
    inline TShowKind ShowProperty() const { return showProperty; }
    inline TShowKind EditProperty() const { return editProperty; }
    inline TString ValueClassProperty() const { return valueClassProperty; }

    inline TCustClass& SetShowClasses(TShowKind value)  { showClasses = value;  return *this; }
    inline TCustClass& SetShowProperty(TShowKind value) { showProperty = value; return *this; }
    inline TCustClass& SetEditProperty(TShowKind value) { editProperty = value; return *this; }
    inline TCustClass& SetValueClassProperty(const TString& value) { valueClassProperty = value; return *this; }
    inline TCustClass& SetCheckPropFun(const TCheckPropFun& value) { checkPropFun = value; return *this; }

    TCustClass& AddType(const TString& typeName, TShowKind value = TShowKind::Parent);
    TCustProp& AddProp(const TString& propName, bool visible = true);
    TCustClass& AddProps(const TString& props);
    TCustClass& AddTypeProp(const TString& typeName, const TString& props);

    TCustClass* Info(const TPropertyManager& value);
    bool CheckType(const TPropertyManager& value);
    bool CheckProp(TPropertyClass* obj, const TString& value);

    void Clear();
    TCustProp& CustProp(const TString& value);
};

class TPropertyEditor{
public:
    TPropertyEditor();
    void Clear();
    virtual void SetObject(TPtrPropertyClass value);

    TPropertyEditor& SetIsAll(bool value = true);
    bool IsAll() const;

    TPropertyEditor& SetIsAllType(bool value = true);
    bool IsAllType() const;

    TPropertyEditor& SetIsAllProperty(bool value = true);
    bool IsAllProperty() const;

    TPropertyEditor& SetIsEdit(bool value = true);
    bool IsEdit() const;

    TObjTree& Tree();
    TCustClass& Info();
protected:
    TPtrPropertyClass obj;
    TCustClass info;
    TObjTree tree;
};


#endif //TESTAPP_PROPERTYEDITOR_H
