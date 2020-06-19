//
// Created by user on 30.09.2019.
//

#ifndef TESTAPP_PROPERTYEDITOR_H
#define TESTAPP_PROPERTYEDITOR_H

#include <deque>
#include "PropertyClass.h"

class TObjTree;
using TFunUpdateTree = std::function<void(TObjTree* value)>;

class TCustInfo{
    bool isAllType = true;
    bool isAllProperty = true;
    bool isEdit = true;
    std::map<TString, TCustInfo> types;
    std::map<TString, bool> props;
public:
    inline bool IsAll() const { return isAllType || isAllProperty; }
    inline bool IsAllType() const { return isAllType; }
    inline bool IsAllProperty() const { return isAllProperty; }

    inline void SetIsAll(bool value) { isAllType = isAllProperty = value; }
    inline void SetIsAllType(bool value) { isAllType = value; }
    inline void SetIsAllProperty(bool value) { isAllProperty = value; }

    inline bool IsEdit() const { return isEdit; }
    inline void SetIsEdit(bool value) { isEdit = value; }

    TCustInfo& AddProp(const TString& name, bool visible);

    bool CheckType(const TString& value)
    {
        if(isAllType) return true;          //если стоит флаг что показывать любой тип
        auto it = types.find(value);        //иначе ищим есть ли такой тип здесь
        if(it == types.end()) return false; //если не нашли значит такой тип не разрешен
        return it->second.isAllProperty || props.size();//если нашли значит должен быть либо все свойства либо какие то из них
    }
    bool CheckProp(const TString& value)
    {
        if(isAllProperty) return true;
        auto it = props.find(value);
        if(it == props.end()) return false;
        return it->second;
    }
};
class TObjTree{
public:
    TObjTree(const TPtrPropertyClass& value = TPtrPropertyClass(), TObjTree* parent = nullptr, int indProp = -1);
    void Clear();//очищаем полностью объект
    void ClearChilds();//очищаем объекты владения
    void SetInfo(TCustInfo* value);
    void SetObj(const TPtrPropertyClass& value);
    const TPtrPropertyClass& Obj() const;
    int IndProp() const;

    void Load(bool refind = false);

    bool IsLoaded() const;
    bool IsChilds() const;//отображает может ли быть объекты в childs
    bool IsProp() const;  //отображает это свойство или класс
    bool IsCheckable() const;
    bool IsColor() const;
    bool IsEnum() const;

    size_t CountProps() const;
    TObjTree& Prop(int index);

    size_t CountChilds() const;
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
    //void SetFunChecked(TChangeThePropertyClass value);
    void SetFunUpdateTree(TFunUpdateTree value);
    using TVecObjTree = TPtrVector<TObjTree>;
private:
    TObjTree* parent = nullptr;
    TCustInfo* info = nullptr;
    TFunUpdateTree update;
    TPtrPropertyClass obj;
    TIdConnect idChange;
    int indProp = -1;

    void CallUpdate();
    TFunUpdateTree GetFunUpdate();

    TVecObjTree props;
    TVecObjTree childs;
    bool isLoaded = false;
    TChangeThePropertyClass funChecked;
    TChangeThePropertyClass FindFunChecked() const;

    bool HasChild(const TVariable& value) const;
    bool HasChild(const TPtrPropertyClass& value) const;

    bool CheckType(const TVariable& value) const;
    bool CheckType(const TPtrPropertyClass& value) const;

    bool CheckProp(const TPropInfo& vale) const;
};

class TPropertyEditor{
public:
    TPropertyEditor();

    virtual void SetObject(TPtrPropertyClass value);

    TPropertyEditor& SetIsAll(bool value = true);
    bool IsAll() const;

    TPropertyEditor& SetIsAllType(bool value = true);
    bool IsAllType() const;

    TPropertyEditor& SetIsAllProperty(bool value = true);
    bool IsAllProperty() const;

    TPropertyEditor& SetIsEdit(bool value = true);
    bool IsEdit() const;

    TCustInfo& Type(const TString& type, bool IsAll = false);//добавляет тип вместе со всеми его свойствами
    TPropertyEditor& Type(const TString& type, const TString);//добавляет тип вместе со всеми его свойствами

    TObjTree& Tree();
    TCustInfo& Info();
protected:
    TPtrPropertyClass obj;
    TCustInfo info;
    TObjTree tree;
};


#endif //TESTAPP_PROPERTYEDITOR_H
