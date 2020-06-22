//
// Created by user on 30.09.2019.
//

#ifndef TESTAPP_PROPERTYEDITOR_H
#define TESTAPP_PROPERTYEDITOR_H

#include <deque>
#include "PropertyClass.h"

class TObjTree;
class TCustInfo;
using TFunUpdateTree = std::function<void(TObjTree* value)>;

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

    bool HasChild(const TPtrPropertyClass& value) const;

    TCustInfo* RootInfo() const;
    TCustInfo* CustInfo(const TPropertyManager& man) const;
};

/*
 * Варианты настройки отображения свойств и классов для корневого объекта (root)
 *          1. Не добавлен ни какой тип,
 *              1.1 свойство IsAllType == true -> Отображаются все объекты child которые есть у root
 *                  1.1.1 без указания свойств у root
 *                      1.1.1.1 свойство IsAllProp == true -> Отображаются все свойства которые есть у child
 *                      1.1.1.2 свойство IsAllProp == false -> Не отображаются свойства которые есть у child
 *                  1.1.2 с указанием свойств у root
 *                      1.1.2.1 свойство IsAllProp == true -> Отображаются все свойства которые есть у child
 *                      1.1.2.2 свойство IsAllProp == false -> Отображаются только те свойства у child которые добавлены
 *              1.2 свойство IsAllType == false -> Не отображается ни один класс которые есть у root
 *                  1.2.1 Так как классов нет свойств отображать не для чего
 *          2. Добавлен тип
 *              2.1 свойство IsAllType == true -> Отображаются все объекты child которые есть у root
 *                  Аналогичен пункту 1.1
 *              2.2 свойство IsAllType == false -> Отображаются все объекты данного типа или типы наследники которые есть у root
 *                  2.2.1 Для типа не указаны свойства
 *                      2.2.1.1 без указания свойств у root
 *                          2.2.1.1.1 свойство IsAllProp == true -> Отображаются все свойства которые есть у child
 *                          2.2.1.1.2 свойство IsAllProp == false -> Не отображаются свойства которые есть у child
 *                      2.2.1.2 с указания свойств у root
 *                          2.2.1.2.1 свойство IsAllProp == true -> Отображаются все свойства которые есть у child
 *                          2.2.1.2.2 свойство IsAllProp == false -> Отображаются свойства которые есть у child указаные у root
 *                  2.2.2 Для типа указаны свойства
 *                      2.2.2.1 у типа IsAllProp  == true -> Отображаются все свойства которые есть у child
 *                      2.2.2.2 у типа IsAllProp  == false -> Отображаются свойства которые есть у child указаныне у типа
 *
 * */
enum class TShowKind{ All, None, Select, Parent};

class TCustInfo{
    TShowKind showClasses = TShowKind::All;
    TShowKind showProperty = TShowKind::All;
    TShowKind editProperty = TShowKind::None;
    std::map<const TPropertyManager*, TCustInfo> types;
    std::map<TString, bool> props;
public:

    inline TShowKind ShowClasses() const { return showClasses; }
    inline TShowKind ShowProperty() const { return showProperty; }
    inline TShowKind EditProperty() const { return editProperty; }

    inline TCustInfo& SetShowClasses(TShowKind value)  { showClasses = value;  return *this; }
    inline TCustInfo& SetShowProperty(TShowKind value) { showProperty = value; return *this; }
    inline TCustInfo& SetEditProperty(TShowKind value) { editProperty = value; return *this; }

    TCustInfo& AddType(const TString& name, TShowKind value = TShowKind::Parent);
    TCustInfo& AddProp(const TString& name, bool visible = true);
    TCustInfo& AddProps(const TString& props);
    TCustInfo& AddTypeProp(const TString& name, const TString& props);

    TCustInfo* Info(const TPropertyManager& value);
    bool CheckType(const TPropertyManager& value);
    bool CheckProp(const TString& value);

    void Clear();
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
    TCustInfo& Info();
protected:
    TPtrPropertyClass obj;
    TCustInfo info;
    TObjTree tree;
};


#endif //TESTAPP_PROPERTYEDITOR_H
