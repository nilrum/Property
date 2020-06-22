//
// Created by user on 30.09.2019.
//

#include "PropertyEditor.h"
TPropertyEditor::TPropertyEditor()
{
    tree.SetInfo(&info);
}

TPropertyEditor &TPropertyEditor::SetIsAll(bool value)
{
    SetIsAllType(value);
    SetIsAllProperty(value);
    return *this;
}

bool TPropertyEditor::IsAll() const
{
    return IsAllType() && IsAllProperty();
}

TPropertyEditor &TPropertyEditor::SetIsAllType(bool value)
{
    info.SetShowClasses(value ? TShowKind::All : TShowKind::Select);
    return *this;
}

bool TPropertyEditor::IsAllType() const
{
    return info.ShowClasses() == TShowKind::All;
}

TPropertyEditor &TPropertyEditor::SetIsAllProperty(bool value)
{
    info.SetShowProperty(value ? TShowKind::All : TShowKind::Select);
    return *this;
}

bool TPropertyEditor::IsAllProperty() const
{
    return info.ShowProperty() == TShowKind::All;
}

TPropertyEditor &TPropertyEditor::SetIsEdit(bool value)
{
    info.SetEditProperty(value ? TShowKind::All : TShowKind::None);
    return *this;
}

bool TPropertyEditor::IsEdit() const
{
    return info.EditProperty() == TShowKind::All;
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

TCustInfo &TPropertyEditor::Info()
{
    return info;
}

void TPropertyEditor::Clear()
{
    tree.Clear();
    info.Clear();
}

//-------------------------------------TObjTree-------------------------------------------------------------------------
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
    TCustInfo* thisInfo = CustInfo(man);
    for(size_t i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsPod())//если есть свойства не класс и не массив
            if(thisInfo->CheckProp(man.Property(i).Name()))
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

bool TObjTree::HasChild(const TPtrPropertyClass &value) const
{
    if(value == nullptr) return false;
    const TPropertyManager& man = value->Manager();

    TCustInfo* thisInfo = CustInfo(man);

    for(size_t i = 0; i < man.CountProperty(); i++)
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
            TPtrPropertyClass ptr = VariableToPropClass(v);
            if(ptr == nullptr) continue;
            if(thisInfo->CheckType(ptr->Manager())) return true;
        }
    return false;
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
    TCustInfo* thisInfo = CustInfo(man);
    for(size_t i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsClass())//если свойства класс
        {
            TPtrPropertyClass ptr = VariableToPropClass(obj->ReadProperty(i));
            if(ptr == nullptr) continue;
            if(thisInfo->CheckType(ptr->Manager()))
                childs.emplace_back(ptr, this, i);
        }
        else if(man.Property(i).IsArray())
            {
                int count = obj->CountInArray(i);
                for(int j = 0; j < count; j++)
                {
                    TPtrPropertyClass ptr = VariableToPropClass(obj->ReadFromArray(i, j));
                    if(ptr == nullptr) continue;
                    if(thisInfo->CheckType(ptr->Manager()))
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

TObjTree *TObjTree::Parent()
{
    return parent;
}

int TObjTree::Num(int def) const
{
    if(parent)
        for(size_t i = 0; i < parent->CountChilds(); i++)
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
    for(size_t i = 0; i < man.CountProperty(); i++)
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
}

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

void TObjTree::SetInfo(TCustInfo *value)
{
    info = value;
}

TCustInfo* TObjTree::RootInfo() const
{
    if(parent) return parent->RootInfo();
    if(info) return info;
    return &Single<TCustInfo>();
}

TCustInfo *TObjTree::CustInfo(const TPropertyManager &man) const
{
    TCustInfo* rootInfo = RootInfo();
    TCustInfo* thisInfo = rootInfo->Info(man);
    if(thisInfo != nullptr)
    {
        if(thisInfo->ShowClasses() != TShowKind::Parent)
            rootInfo = thisInfo;
    }
    return rootInfo;
}

//----------------------------------------------------------------------------------------------------------------------

bool TCustInfo::CheckType(const TPropertyManager& value)
{
    switch(showClasses)
    {
        case TShowKind::All: return true;
        case TShowKind::Select: return Info(value) != nullptr;
        default : return false;
    }
}

bool TCustInfo::CheckProp(const TString& value)
{
    switch(showProperty)
    {
        case TShowKind::All:
            return true;

        case TShowKind::Select:
        {
            auto it = props.find(value);
            if (it != props.end())
                return it->second;
            return false;
        }

        default :
            return false;
    }
}

TCustInfo& TCustInfo::AddType(const TString &name, TShowKind value)
{
    const auto& man = TPropertyManager::Manager(name);
    if(man.IsValid())
        return types[&man] = TCustInfo().SetShowProperty(value);
    else
        return *this;
}

TCustInfo& TCustInfo::AddProp(const TString &name, bool visible)
{
    props[name] = visible;
    return *this;
}

TCustInfo &TCustInfo::AddProps(const TString &props)
{
    TVecString propInfos = Split(props, ',');
    for(const auto& prop : propInfos)
    {
        TVecString propInfo = Split(prop, ':');
        switch (propInfo.size())
        {
            case 1://указали просто имя
                AddProp(Trim(propInfo[0]));
                break;

            case 2://указали имя и видимость
                AddProp(Trim(propInfo[0]), TVariable(propInfo[1]).ToBool());
                break;

            default:
                continue;
        }
    }
    return *this;
}

TCustInfo &TCustInfo::AddTypeProp(const TString &name, const TString &props)
{
    return AddType(name, TShowKind::Select).AddProps(props);
}

TCustInfo* TCustInfo::Info(const TPropertyManager &value)
{
    auto it = types.find(&value);        //иначе ищим есть ли такой тип здесь
    if(it != types.end()) return &it->second;

    for (auto &it : types)
    {
        if (value.IsCustableTo(*it.first))
            return &it.second;
    }
    return nullptr;
}

void TCustInfo::Clear()
{
    props.clear();
    types.clear();
}




