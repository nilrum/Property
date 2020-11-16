//
// Created by user on 30.09.2019.
//

#include "PropertyEditor.h"
#include "History.h"

class THistoryItemEditor : public THistoryItem{
public:
    THistoryItemEditor(TPtrPropertyClass ptr, int ind):obj(ptr), indProp(ind)
    {
        name = STDFORMAT("Edit \"%s\" property", STR(ptr->Manager().Property(ind).Name()));
        value = ptr->ReadProperty(ind);
    }

    void Back() override
    {
        if(obj.expired()) return;
        auto lock = obj.lock();
        auto buf = lock->ReadProperty(indProp);
        lock->WriteProperty(indProp, value);
        value = buf;//сохраняем для Next
    }

    void Next() override
    {
        Back();
    }
private:
    TWPtrPropertyClass obj;
    int indProp = -1;
    TVariable value;
};

TPropertyEditor::TPropertyEditor(const TPtrObjTree& t)
{
    SetTree(t);
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
    return true;
}

void TPropertyEditor::SetObject(const TPtrPropertyClass& value)
{
    tree->Clear();
    tree->SetObj(value);
}

TObjTree &TPropertyEditor::Tree()
{
    return *tree;
}

TCustClass &TPropertyEditor::ClassCustoms()
{
    return classCustoms;
}

void TPropertyEditor::Clear()
{
    tree->Clear();
    classCustoms.Clear();
}

TWPtrPropertyClass TPropertyEditor::Obj() const
{
    return tree->Obj();
}

TPtrPropertyClass TPropertyEditor::LockObj() const
{
    return tree->LockObj();
}

void TPropertyEditor::SetTree(const TPtrObjTree &value)
{
    tree = value;
    tree->SetCustomClass(&classCustoms);
}

//-------------------------------------TObjTree-------------------------------------------------------------------------
TObjTree::TObjTree(const TConstWPtrObjTree& par, int ind): parent(par), indProp(ind)
{
}

void TObjTree::Clear()
{
    items.clear();
    isLoaded = false;
    idChange.disconnect();
    idDelete.disconnect();
    obj.reset();
}

void TObjTree::SetObj(const TPtrPropertyClass& value)
{
    if(obj.expired() == false)
    {
        auto lock = LockObj();
        if(value != lock)
            Clear();
    }
    obj = value;
    if(value != nullptr && IsProp() == false)
    {
        idChange = value->OnChanged.connect([this]()
            {
                if (isLoaded == false) return;
                RescanItems();
            });
        if(Parent().expired())//если родителя нет значит корневой объект
            idDelete = value->OnDeleting.connect([this](TPropertyClass*)
                {
                    BeginDelete(this);
                    Clear();
                    EndDelete(this);
                });
    }
}

const TPtrObjTree &TObjTree::Item(size_t index) const
{
    return items[index];
}

size_t TObjTree::CountItems(bool autoLoad) const
{
    if(isLoaded == false && autoLoad) LoadItems();
    return items.size();
}

void TObjTree::RescanItems()
{
    if(items.size())
    {
        BeginDelete(this);
        items.clear();
        EndDelete(this);
    }
    bool isCall = isLoaded;
    if(isCall) BeginAdd(this);
    LoadItems();
    if(isCall) EndAdd(this);
}

void TObjTree::LoadItems() const
{
    if(obj.expired()) return;
    auto lock = obj.lock();

    if(IsProp()) return;

    const TPropertyManager& man = lock->Manager();
    TCustClass* propsInfo = PropCustoms();
    for(size_t i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsPod())//если есть свойства не класс и не массив
            if(propsInfo->CheckProp(lock.get(), man.Property(i).Name()))
                items.emplace_back(new TObjTree(weak_from_this(), i))->SetObj(lock);


    TCustClass* classInfo = ClassCustoms(man, true);
    for(size_t i = 0; i < man.CountProperty(); i++)
    {
        const TPropInfo& info = man.Property(i);
        if (info.IsClass())//если свойства класс
        {
            TPtrPropertyClass ptr = VariableToPropClass(lock->ReadProperty(i));
            if (ptr == nullptr) continue;
            if (classInfo->CheckType(ptr->Manager(), info.Name()))
                items.emplace_back(new TObjTree(weak_from_this(), i))->SetObj(ptr);
        }
        else if (info.IsArray())
        {
            int count = lock->CountInArray(i);
            for (int j = 0; j < count; j++)
            {
                TPtrPropertyClass ptr = VariableToPropClass(lock->ReadFromArray(i, j));
                if (ptr == nullptr) continue;
                if (classInfo->CheckType(ptr->Manager(), info.Name()))
                    items.emplace_back(new TObjTree(weak_from_this(), i))->SetObj(ptr);
            }
        }
    }
    isLoaded = true;
}

bool TObjTree::HasProp(const TPtrPropertyClass &value) const
{
    if(value == nullptr) return false;
    const TPropertyManager& man = value->Manager();
    TCustClass* propsInfo = PropCustoms();
    for(size_t i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsPod())//если есть свойства не класс и не массив
            if(propsInfo->CheckProp(value.get(), man.Property(i).Name()))
               return true;
    return false;
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

void TObjTree::AddChild(TPtrPropertyClass value, int indProp)
{
    auto lock = LockObj();
    if(lock)
        lock->AddToArray(indProp, PropertyClassToVariable(value));
}

void TObjTree::DelChild(TPtrPropertyClass value, int indProp)
{
    auto lock = LockObj();
    if(lock)
        lock->DelFromArray(indProp, PropertyClassToVariable(value));
}


bool TObjTree::IsProp() const
{
    TPtrPropertyClass lock;
    return IsProp(lock);
}

bool TObjTree::IsProp(TPtrPropertyClass &lock) const
{
    lock = LockObj();
    if(indProp == -1 || lock == nullptr || parent.expired()) return false;
    auto parObj = LockParent()->LockObj();
    if(parObj == nullptr) return false;
    return parObj->Manager().Property(indProp).IsPod();
}

bool TObjTree::IsLoaded() const
{
    return isLoaded;
}

bool TObjTree::HasChildren() const
{
    TPtrPropertyClass lock;
    if(IsProp(lock)) return false;
    return items.size() || HasProp(lock) || HasChild(lock);
}

bool TObjTree::IsColor() const
{
    TPtrPropertyClass lock;//для IsProp == true неважно какой obj брать
    return IsProp(lock) && lock->Manager().Property(indProp).Type() == "TColor";
}

bool TObjTree::IsBool() const
{
    TPtrPropertyClass lock;
    return IsProp(lock) && lock->Manager().Property(indProp).Type() == "bool";
}

bool TObjTree::IsEditable() const
{
    TPtrPropertyClass lock;
    return IsProp(lock) && lock->Manager().Property(indProp).IsReadOnly() == false;
}

int TObjTree::Num(int def) const
{
    if(parent.expired() == false)
    {
        auto lock = LockParent();
        for (size_t i = 0; i < lock->CountItems(); i++)
            if (lock->Item(i).get() == this) return i;
    }
    return def;
}

TString TObjTree::Name() const
{
    if(obj.expired()) return TString();
    auto lock = LockObj();
    if(indProp == -1)
    {
        if(lock->Name().empty()) return "Name";
        else return lock->Name();
    }
    else
    {
        if(parent.expired()) return TString();
        auto lockPar = LockParent();
        const TPropInfo & info = lockPar->LockObj()->Manager().Property(indProp);//предполагается что ребенок не может быть без родителя

        if(info.IsArray() && lock->Name().empty() == false)//для элементов массива возвращаем его имя если есть
            return lock->Name();
        return info.Name();
    }
}

TVariable TObjTree::Value(bool isType) const
{
    TPtrPropertyClass lock;
    if(IsProp(lock))
        return lock->ReadProperty(indProp);
    else
    {
        if(lock == nullptr) return TVariable();
        if(isType)
            return (lock->TypeClass() + "::" + lock->Name()).c_str();
        else
            return lock->ReadProperty(RootInfo()->ValueClassProperty());
    }
}

void TObjTree::SetValue(const TVariable &value)
{
    TPtrPropertyClass lock;
    if(IsProp(lock))
    {
        if(THistory::IsUsed())
            THistory::Single()->AddItem(std::make_shared<THistoryItemEditor>(lock, indProp));
        lock->WriteProperty(indProp, value);
    }
}

TObjTree::TVectArrayInfo TObjTree::ArrayInfo() const
{
    TVectArrayInfo res;
    if(obj.expired()) return res;
    const TPropertyManager& man = LockObj()->Manager();
    for(size_t i = 0; i < man.CountProperty(); i++)
        if(man.Property(i).IsArray())
            res.emplace_back(man.Property(i).Name(), i);
    return res;
}

bool TObjTree::IsCheckable() const
{
    TPtrPropertyClass lock;
    return IsProp(lock) == false && lock && lock->IndexProperty("isUsed") != -1;
}

bool TObjTree::IsChecked() const
{
    TPtrPropertyClass lock = LockObj();
    if(lock == nullptr) return false;
    int index = lock->IndexProperty("isUsed");
    if(index == -1) return false;
    return lock->ReadProperty(index).ToBool();
}

void TObjTree::SetIsChecked(bool value)
{
    TPtrPropertyClass lock = LockObj();
    if(lock == nullptr) return;
    int index = lock->IndexProperty("isUsed");
    if(index == -1) return;
    lock->WriteProperty(index, value);
}

void TObjTree::SetCustomClass(TCustClass *value)
{
    info = value;
}

TCustClass* TObjTree::RootInfo() const
{
    if(parent.expired() == false) return LockParent()->RootInfo();
    if(info) return info;
    return &Single<TCustClass>();
}

TCustClass *TObjTree::ClassCustoms(bool checkClass) const
{
    if(obj.expired()) return RootInfo();
    return ClassCustoms(LockObj()->Manager(), checkClass);
}

TCustClass *TObjTree::ClassCustoms(const TPropertyManager &man, bool checkClass) const
{
    TCustClass* rootInfo = RootInfo();

    TCustClass* parInfo = (parent.expired()) ? rootInfo : LockParent()->ClassCustoms(IsProp() == false) ;
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
    return ClassCustoms(false);
}

void TObjTree::BeginDelete(TObjTree *objTree)
{
    if(parent.expired()) return;
    TObjTree* p = (TObjTree*)(LockParent().get());
    p->BeginDelete(objTree);
}

void TObjTree::EndDelete(TObjTree *objTree)
{
    if(parent.expired()) return;
    TObjTree* p = (TObjTree*)(LockParent().get());
    p->EndDelete(objTree);
}

void TObjTree::BeginAdd(TObjTree *objTree)
{
    if(parent.expired()) return;
    TObjTree* p = (TObjTree*)(LockParent().get());
    p->BeginAdd(objTree);
}

void TObjTree::EndAdd(TObjTree *objTree)
{
    if(parent.expired()) return;
    TObjTree* p = (TObjTree*)(LockParent().get());
    p->EndAdd(objTree);
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






