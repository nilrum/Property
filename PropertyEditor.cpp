//
// Created by user on 30.09.2019.
//

#include "PropertyEditor.h"
TPropertyEditor::TPropertyEditor()
{
    tree.SetCustomClass(&classCustoms);
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
    classCustoms.SetShowClasses(value ? TShowClass::All : TShowClass::SelType);
    return *this;
}

bool TPropertyEditor::IsAllType() const
{
    return classCustoms.ShowClasses() == TShowClass::All;
}

TPropertyEditor &TPropertyEditor::SetIsAllProperty(bool value)
{
    classCustoms.SetShowProperty(value ? TShowProp::All : TShowProp::Select);
    return *this;
}

bool TPropertyEditor::IsAllProperty() const
{
    return classCustoms.ShowProperty() == TShowProp::All;
}

TPropertyEditor &TPropertyEditor::SetIsEdit(bool value)
{
    classCustoms.SetEditProperty(value ? TShowProp::All : TShowProp::None);
    return *this;
}

bool TPropertyEditor::IsEdit() const
{
    return classCustoms.EditProperty() == TShowProp::All;
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

TCustClass &TPropertyEditor::ClassCustoms()
{
    return classCustoms;
}

void TPropertyEditor::Clear()
{
    tree.Clear();
    classCustoms.Clear();
}

TPtrPropertyClass TPropertyEditor::Obj() const
{
    return tree.Obj();
}

//-------------------------------------TObjTree-------------------------------------------------------------------------
TObjTree::TObjTree(const TPtrPropertyClass &value, TObjTree* p, int ind):parent(p), indProp(ind)
{
    SetObj(value);
}

void TObjTree::Clear()
{
    ClearChildren();
    props.clear();
    idChange.disconnect();
    obj.reset();
}

void TObjTree::ClearChildren()
{
    children.clear();
    isLoaded = false;
}

void TObjTree::SetObj(const TPtrPropertyClass& value)
{
    obj = value;
    if(obj == nullptr || IsProp()) return;
    idChange = obj->OnChanged.connect(&TObjTree::CallUpdate, this);
    const TPropertyManager& man = value->Manager();
    TCustClass* thisInfo = PropCustoms();//CustInfo(man, false);
    for(size_t i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsPod())//если есть свойства не класс и не массив
            if(thisInfo->CheckProp(obj.get(), man.Property(i).Name()))
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

    TCustClass* thisInfo = ClassCustoms(man, true);

    for(size_t i = 0; i < man.CountProperty(); i++)
    {
        const TPropInfo& info = man.Property(i);
        if (info.IsPod() == false)//если есть свойства класс или массив
        {
            TVariable v;
            if (info.IsArray() == false)
            {
                v = value->ReadProperty(i);
            }
            else
            {
                if (value->CountInArray(i))//если в массиве есть элементы
                    v = value->ReadFromArray(i, 0);
            }
            TPtrPropertyClass ptr = VariableToPropClass(v);
            if (ptr == nullptr) continue;
            if (thisInfo->CheckType(ptr->Manager(), info.Name())) return true;
        }
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

size_t TObjTree::CountChildren() const
{
    return children.size();
}

TObjTree &TObjTree::Child(int index)
{
    return children[index];
}

const TObjTree &TObjTree::Child(int index) const
{
    return children[index];
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
    ClearChildren();
    if(obj == nullptr) return;
    const TPropertyManager& man = obj->Manager();
    TCustClass* thisInfo = ClassCustoms(man, true);
    for(size_t i = 0; i < man.CountProperty(); i++)
    {
        const TPropInfo& info = man.Property(i);
        if (info.IsClass())//если свойства класс
        {
            TPtrPropertyClass ptr = VariableToPropClass(obj->ReadProperty(i));
            if (ptr == nullptr) continue;
            if (thisInfo->CheckType(ptr->Manager(), info.Name()))
                children.emplace_back(ptr, this, i);
        }
        else if (info.IsArray())
        {
            int count = obj->CountInArray(i);
            for (int j = 0; j < count; j++)
            {
                TPtrPropertyClass ptr = VariableToPropClass(obj->ReadFromArray(i, j));
                if (ptr == nullptr) continue;
                if (thisInfo->CheckType(ptr->Manager(), info.Name()))
                    children.emplace_back(ptr, this, i);
            }
        }
    }
    isLoaded = true;
}

bool TObjTree::IsLoaded() const
{
    return isLoaded;
}

bool TObjTree::IsChildren() const
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

bool TObjTree::IsBool() const
{
    return IsProp() && parent->Obj()->Manager().Property(indProp).Type() == "bool";
}

bool TObjTree::IsEditable() const
{
    return IsProp() && parent->Obj()->Manager().Property(indProp).IsReadOnly() == false;
}


TObjTree *TObjTree::Parent()
{
    return parent;
}

int TObjTree::Num(int def) const
{
    if(parent)
        for(size_t i = 0; i < parent->CountChildren(); i++)
            if(&parent->Child(i) == this) return i;
    return def;
}

int TObjTree::LoadedCount()
{
    if(isLoaded == false) Load();
    return CountChildren();
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
            return obj->ReadProperty(RootInfo()->ValueClassProperty());
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

void TObjTree::SetOnUpdateTree(TOnUpdateTree value)
{
    update = value;
}

void TObjTree::CallUpdate()
{
    TOnUpdateTree call = GetFunUpdate();
    if(call) call(this);
}

TOnUpdateTree TObjTree::GetFunUpdate()
{
    if(parent != nullptr) return parent->GetFunUpdate();
    return update;
}

void TObjTree::SetCustomClass(TCustClass *value)
{
    info = value;
}

TCustClass* TObjTree::RootInfo() const
{
    if(parent) return parent->RootInfo();
    if(info) return info;
    return &Single<TCustClass>();
}

TCustClass *TObjTree::ClassCustoms(bool checkClass) const
{
    return ClassCustoms(obj->Manager(), checkClass);
}

TCustClass *TObjTree::ClassCustoms(const TPropertyManager &man, bool checkClass) const
{
    TCustClass* rootInfo = RootInfo();

    TCustClass* parInfo = (parent) ? parent->ClassCustoms(IsProp() == false) : rootInfo;
    if(IsProp()) return parInfo;
    TCustClass* thisInfo = parInfo->Info(man);
    if(thisInfo && (
            thisInfo->ShowClasses()  != TShowClass::Parent && checkClass ||
            thisInfo->ShowProperty() != TShowProp::Parent && checkClass == false
            )
      )
        return thisInfo;
    return parInfo;
}

TCustClass *TObjTree::PropCustoms() const
{
    return ClassCustoms(obj->Manager(), false);
}



//----------------------------------------------------------------------------------------------------------------------

bool TCustClass::CheckType(const TPropertyManager& value, const TString& nameProp)
{
    switch(showClasses)
    {
        case TShowClass::All: return true;
        case TShowClass::SelType: return Info(value) != nullptr;
        case TShowClass::SelProp: return Info(value) != nullptr && props.find(nameProp) != props.end();
        default : return false;
    }
}

bool TCustClass::CheckProp(TPropertyClass* obj, const TString& value)
{
    switch(showProperty)
    {
        case TShowProp::All:
            return true;

        case TShowProp::Select:
        {
            auto it = props.find(value);
            if (it != props.end())
                return it->second.visible;
            return false;
        }
        case TShowProp::SelTrue:
        {
            auto it = props.find(value);
            if (it != props.end())
                return it->second.visible;
            return true;
        }
        case TShowProp::Function:
            if(checkPropFun)
                return checkPropFun(obj, value);
            else
                return false;
        default :
            return false;
    }
}

TCustClass& TCustClass::AddType(const TString &typeName, TShowProp sp, TShowClass sc)
{
    return AddType(typeName, TCustClass().SetShowProperty(sp).SetShowClasses(sc));
}

TCustClass &TCustClass::AddType(const TString &typeName, const TCustClass &value)
{
    const auto& man = TPropertyManager::Manager(typeName);
    if(man.IsValid())
        return types[&man] = value;
    else
        return *this;
}


TCustProp& TCustClass::AddProp(const TString &propName, bool visible)
{
    TCustProp& res = props[propName];
    res.visible = visible;
    return res;
}

TCustClass &TCustClass::AddProps(const TString &props)
{
    TVecString propInfos = Split(props, ',');
    for(const auto& prop : propInfos)
    {
        TVecString propInfo = Split(prop, ':');
        if(propInfo.size() == 0) continue;
        TCustProp& cust = AddProp(Trim(propInfo[0]));
        for(size_t i = 1; i < propInfo.size(); i++)
        {
            TString& pi = propInfo[i];
            if (pi[0] == '[')
            {
                //убираем скобки и делим на мин макс
                TVecString minmax = Split(pi.substr(1, pi.size() - 2), ';');
                if(minmax.size()) cust.SetMin(std::stod(minmax[0]));
                if(minmax.size() > 1) cust.SetMax(std::stod(minmax[1]));
            }
            else if (pi[0] == '%')
                cust.SetFormat(propInfo[i]);
            else
                cust.SetVisible(TVariable(pi).ToBool());
        }
    }
    return *this;
}

TCustClass &TCustClass::AddTypeProp(const TString &typeName, const TString &props)
{
    return AddType(typeName).AddProps(props);
}

TCustClass *TCustClass::Info(const TString &propMan)
{
    return Info(TPropertyManager::Manager(propMan));
}

TCustClass* TCustClass::Info(const TPropertyManager &value)
{
    auto it = types.find(&value);        //ищим есть ли такой тип здесь
    if(it != types.end()) return &it->second;

    for (auto &it : types)
    {
        if (value.IsCustableTo(*it.first))
            return &it.second;
    }
    return nullptr;
}

void TCustClass::Clear()
{
    props.clear();
    types.clear();
}

TCustProp &TCustClass::CustProp(const TString &value)
{
    auto it = props.find(value);
    if(it != props.end()) return it->second;
    return Single<TCustProp>();
}






