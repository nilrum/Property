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
    idChange.disconnect();
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
    if(obj == nullptr || IsProp()) return;
    idChange = obj->OnChange().connect([this](){ CallUpdate(); });
    const TPropertyManager& man = value->Manager();
    for(int i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsPod())//если есть свойства не класс и не массив
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
        if(man.Property(i).IsPod() == false)//если есть свойства класс или массив
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
}

void TObjTree::DelChild(TPtrPropertyClass value, int indProp)
{
    obj->DelFromArray(indProp, PropertyClassToVariable(value));
}

void TObjTree::DelChild(TObjTree* value)
{
    DelChild(value->Obj(), value->IndProp());
}

void TObjTree::Load(bool refind)
{
    if(isLoaded && refind == false) return;
    ClearChilds();
    if(obj == nullptr) return;
    const TPropertyManager& man = obj->Manager();
    for(int i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsClass())//если свойства класс
        {
            TPtrPropertyClass ptr = VariableToPropClass(obj->ReadProperty(i));
            if(CheckType(ptr))
                childs.emplace_back(ptr, this, i);
        }
        else if(man.Property(i).IsArray())
            {
                int count = obj->CountInArray(i);
                for(int j = 0; j < count; j++)
                {
                    TPtrPropertyClass ptr = VariableToPropClass(obj->ReadFromArray(i, j));
                    if(CheckType(ptr))
                        childs.emplace_back(ptr, this, i);
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
    if(indProp != -1 && parent->Obj()->Manager().Property(indProp).IsPod()) return false;
    return HasChild(obj);
}

bool TObjTree::IsProp() const
{
    return indProp != -1 && parent && parent->Obj()->Manager().Property(indProp).IsPod();
}

bool TObjTree::IsColor() const
{
    return IsProp() && parent->Obj()->Manager().Property(indProp).Type() == "TColor";
}

bool TObjTree::IsEnum() const
{
    return false;
}



bool TObjTree::CheckProp(const TPropInfo& value) const
{
    return true;
}

TObjTree *TObjTree::Parent()
{
    return parent;
}

int TObjTree::Num(int def) const
{
    if(parent)
        for(int i = 0; i < parent->CountChilds(); i++)
            if(&parent->Child(i) == this) return i;
    return def;
}

int TObjTree::LoadedCount()
{
    if(isLoaded == false) Load();
    return CountChilds();
}

TString TObjTree::Name() const
{
    if(indProp == -1)
    {
        if(obj->Name().empty()) return "Name";
        else return obj->Name();
    }
    else
    {
        const TPropInfo & info = parent->Obj()->Manager().Property(indProp);
        TString rez = info.Name();
        if(info.IsArray() && obj->Name().empty() == false)//для элементов массива возвращаем его имя если есть
            rez = obj->Name();
        return rez;
    }
}

TVariable TObjTree::Value(bool isType) const
{
    if(IsProp())
        return obj->ReadProperty(indProp);
    else
    {
        if(isType)
            return (obj->TypeClass() + "::" + obj->Name()).c_str();
        else
            return TString();
    }
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

bool TObjTree::IsCheckable() const
{
    return IsProp() == false && obj->IndexProperty("isUsed") != -1;
}

bool TObjTree::IsChecked() const
{
    int index = obj->IndexProperty("isUsed");
    if(index == -1) return false;
    return obj->ReadProperty(index).ToBool();
}

void TObjTree::SetIsChecked(bool value)
{
    int index = obj->IndexProperty("isUsed");
    if(index == -1) return;
    obj->WriteProperty(index, value);
    /*TChangeThePropertyClass fun = FindFunChecked();//TODO Remove this
    if(fun)
    {
        TString fullName = obj->Name();
        TObjTree* p = parent;
        while(p != nullptr)
        {
            fullName = p->obj->Name() + "/" + fullName;
            p = p->Parent();
        }
        fun((value) ? obj : TPtrPropertyClass(), fullName);
    }*/
}

/*void TObjTree::SetFunChecked(TChangeThePropertyClass value)
{
    funChecked = value;
}*/

TChangeThePropertyClass TObjTree::FindFunChecked() const
{
    if(funChecked) return funChecked;
    else if(parent) return parent->FindFunChecked();

    return TChangeThePropertyClass();
}

int TObjTree::LoadedCountAll()
{
    return CountProps() + LoadedCount();
}

void TObjTree::SetFunUpdateTree(TFunUpdateTree value)
{
    update = value;
}

void TObjTree::CallUpdate()
{
    TFunUpdateTree call = GetFunUpdate();
    if(call) call(this);
}

TFunUpdateTree TObjTree::GetFunUpdate()
{
    if(parent != nullptr) return parent->GetFunUpdate();
    return update;
}




