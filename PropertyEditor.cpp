//
// Created by user on 30.09.2019.
//

#include "PropertyEditor.h"

TPropertyEditor &TPropertyEditor::SetIsAll(bool value)
{
    SetIsAllType(value);
    SetIsAllProperty(value);
    return *this;
}

bool TPropertyEditor::IsAll() const
{
    return isAllType && isAllProperty;
}

TPropertyEditor &TPropertyEditor::SetIsAllType(bool value)
{
    isAllType = value;
    return *this;
}

bool TPropertyEditor::IsAllType() const
{
    return isAllType;
}

TPropertyEditor &TPropertyEditor::SetIsAllProperty(bool value)
{
    isAllProperty = value;
    return *this;
}

bool TPropertyEditor::IsAllProperty() const
{
    return isAllProperty;
}

TPropertyEditor &TPropertyEditor::SetIsEdit(bool value)
{
    isEdit = value;
    return *this;
}

bool TPropertyEditor::IsEdit() const
{
    return isEdit;
}

void TPropertyEditor::SetObject(TPtrPropertyClass value)
{
    tree.Clear();
    if(value)
        tree.SetObj(std::move(value));
}

TObjTree &TPropertyEditor::Tree()
{
    return tree;
}

//TObjTree
TObjTree::TObjTree(const TPtrPropertyClass &value, TObjTree* p, int ind):parent(p), indProp(ind)
{
    SetObj(value);
}

void TObjTree::Clear()
{
    ClearChilds();
    props.clear();
    obj.reset();
}

void TObjTree::ClearChilds()
{
    childs.clear();
    isLoaded = false;
}

void TObjTree::SetObj(const TPtrPropertyClass& value)
{
    obj = value;
    if(static_cast<bool>(obj) == false || IsProp()) return;
    const TPropertyManager& man = value->Manager();
    for(int i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsClass() == false && man.Property(i).IsArray() == false)//если есть свойства не класс и не массив
            if(CheckProp(man.Property(i)))
                props.emplace_back(obj, this, i);
}

const TPtrPropertyClass &TObjTree::Obj() const
{
    return obj;
}


int TObjTree::IndProp() const
{
    return indProp;
}


bool TObjTree::HasChild(const TVariable &value) const
{
    return HasChild(VariableToPropClass(value));
}

bool TObjTree::HasChild(const TPtrPropertyClass &value) const
{
    if(static_cast<bool>(value) == false) return false;
    const TPropertyManager& man = value->Manager();
    for(int i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsClass())//если есть свойства класс или массив
        {
            TVariable v;
            if(man.Property(i).IsArray() == false)
            {
                v = value->ReadProperty(i);
            }
            else
            {
                if(value->CountInArray(i))//если в массиве есть элементы
                    v = value->ReadFromArray(i, 0);
            }
            if(CheckType(v)) return true;
        }
    return false;
}

bool TObjTree::CheckType(const TVariable &value) const
{
    return CheckType(VariableToPropClass(value));
}

bool TObjTree::CheckType(const TPtrPropertyClass &value) const
{
    if(static_cast<bool>(value) == false) return false;
    return true;
}

size_t TObjTree::CountProps() const
{
    return props.size();
}

TObjTree &TObjTree::Prop(int index)
{
    return props[index];
}

size_t TObjTree::CountChilds() const
{
    return childs.size();
}

TObjTree &TObjTree::Child(int index)
{
    return childs[index];
}

const TObjTree &TObjTree::Child(int index) const
{
    return childs[index];
}

void TObjTree::AddChild(TPtrPropertyClass value, int indProp)
{
    obj->AddToArray(indProp, PropertyClassToVariable(value));
    childs.emplace_back(value, this, indProp);
}

void TObjTree::DelChild(TPtrPropertyClass value, int indProp)
{
    childs.erase(
            std::find_if(childs.begin(), childs.end(),
                    [value](const TObjTree& it){ return it.Obj() == value; }
                    )
                    );
    obj->DelFromArray(indProp, PropertyClassToVariable(value));
}

void TObjTree::DelChild(int index)
{
    DelChild(childs[index].Obj(), childs[index].IndProp());
}

void TObjTree::Load()
{
    if(isLoaded) return;
    ClearChilds();
    if(static_cast<bool>(obj) == false) return;
    const TPropertyManager& man = obj->Manager();
    for(int i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsClass())//если свойства класс или массив
        {
            if(man.Property(i).IsArray() == false)
            {
                TPtrPropertyClass ptr = VariableToPropClass(obj->ReadProperty(i));
                if(CheckType(ptr))
                    childs.emplace_back(ptr, this, i);
            }
            else
            {
                int count = obj->CountInArray(i);
                for(int j = 0; j < count; j++)
                {
                    TPtrPropertyClass ptr = VariableToPropClass(obj->ReadFromArray(i, j));
                    if(CheckType(ptr))
                        childs.emplace_back(ptr, this, i);
                }
            }
        }
    isLoaded = true;
}

bool TObjTree::IsLoaded() const
{
    return isLoaded;
}

bool TObjTree::IsChilds() const
{
    if(indProp != -1 && parent->Obj()->Manager().Property(indProp).IsClass() == false) return false;
    return HasChild(obj);
}

bool TObjTree::IsProp() const
{
    return indProp != -1 && parent && parent->Obj()->Manager().Property(indProp).IsClass() == false;
}


bool TObjTree::CheckProp(const TPropInfo& value) const
{
    return true;
}

TObjTree *TObjTree::Parent()
{
    return parent;
}

int TObjTree::Num() const
{
    if(parent)
        for(int i = 0; i < parent->CountChilds(); i++)
            if(&parent->Child(i) == this) return i;
    return 0;
}

int TObjTree::LoadedCount()
{
    if(isLoaded == false) Load();
    return CountChilds();
}

TString TObjTree::Name() const
{
    if(indProp == -1)
        return "Name";
    else
        return parent->Obj()->Manager().Property(indProp).Name();
}

TVariable TObjTree::Value() const
{
    //если indProp != -1 то parent должен быть обязательно
    if(IsProp())
        return obj->ReadProperty(indProp);
    else
        return (obj->TypeClass() + "::" + obj->Name()).c_str();
}

void TObjTree::SetValue(const TVariable &value)
{
    if(IsProp())
        obj->WriteProperty(indProp, value);
}

TObjTree::TVectArrayInfo TObjTree::ArrayInfo() const
{
    TVectArrayInfo rez;
    const TPropertyManager& man = obj->Manager();
    for(int i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsArray())
            rez.emplace_back(man.Property(i).Name(), i);
    return rez;
}






