//
// Created by user on 22.08.2019.
//

#include "PropertyClass.h"

INIT_PROPERTYS(TPropertyClass)

TPropertyManager::TPropertyManager(const TString &_type, TFunCreate &&_createType) : type(_type),
                                                                                     createTypeFun(_createType)
{
    All()[type] = this;
}

TPropertyManager::TFunCreate TPropertyManager::CreateTypeFun() const
{
    return createTypeFun;
}

TPtrPropertyClass TPropertyManager::CreateObj() const
{
    if (createTypeFun) return createTypeFun();
    return TPtrPropertyClass();
}

TPropInfo &TPropertyManager::AddProperty(const TString &type, const TString &name, bool isClass)
{
    AddProperty(TPropInfo(name, type, isClass));
    return properties.back();
}

void TPropertyManager::AppendProperties(TPropertyManager &value)
{
    baseManager = &value;
    baseManager->AddChildManager(*this);

    for (auto info : value.properties)
        AddProperty(TPropInfo(info));
}

void TPropertyManager::AddProperty(TPropInfo &&value)
{
    hashProperties[value.Name()] = properties.size();
    properties.push_back(value);
}

const TPropInfo &TPropertyManager::Property(int index) const
{
    if (CheckIndex(index))
        return properties[index];
    else
        return Single<TPropInfo>();
}

const TPropInfo &TPropertyManager::FindProperty(const TString &nameProperty) const
{
    return Property(IndexProperty(nameProperty));
}

int TPropertyManager::IndexProperty(const TString &nameProperty) const
{
    auto it = hashProperties.find(nameProperty);
    if (it != hashProperties.end())
        return it->second;
    return -1;
}

size_t TPropertyManager::CountProperty() const
{
    return properties.size();
}

TString TPropertyManager::Type() const
{
    return type;
}

bool TPropertyManager::IsInit() const
{
    return isInit;
}

void TPropertyManager::SetIsInit(bool value)
{
    isInit = value;
}

TVariable TPropertyManager::ReadProperty(int index, const TPropertyClass *obj) const
{
    if (CheckGet(index))
        return properties[index].CallGet(obj);
    return TVariable();
}

void TPropertyManager::WriteProperty(int index, TPropertyClass *obj, const TVariable &value) const
{
    if (CheckSet(index))
        properties[index].CallSet(obj, value);
}

bool TPropertyManager::CheckIndex(int index) const
{
    return index >= 0 && static_cast<size_t>(index) < properties.size();
}

bool TPropertyManager::CheckGet(int index) const
{
    return CheckIndex(index) && properties[index].IsValid();
}

bool TPropertyManager::CheckSet(int index) const
{
    return CheckIndex(index) && properties[index].IsValid() && properties[index].IsReadOnly() == false;
}

int TPropertyManager::ReadCountInArray(int index, const TPropertyClass *obj) const
{
    if (CheckGet(index) && properties[index].IsArray())
        return properties[index].CallGetCountArray(obj);
    return 0;
}

TVariable TPropertyManager::ReadFromArray(int index, const TPropertyClass *obj, int indexArray) const
{
    if (CheckGet(index) && properties[index].IsArray())
        return properties[index].CallGetArray(obj, indexArray);
    return TVariable();
}

void TPropertyManager::AddToArray(int index, TPropertyClass *obj, const TVariable &value)
{
    if (CheckSet(index) && properties[index].IsArray())
        properties[index].CallAddArray(obj, value);
}

void TPropertyManager::DelFromArray(int index, TPropertyClass *obj, const TVariable &value)
{
    if (CheckSet(index) && properties[index].IsArray())
        properties[index].CallDelArray(obj, value);
}

size_t TPropertyManager::CountManagers()
{
    return All().size();
}

const TPropertyManager &TPropertyManager::Manager(const TString &type)
{
    auto it = All().find(type);
    if (it != All().end()) return *it->second;
    return Single<TPropertyManager>();
}

void TPropertyManager::AddChildManager(TPropertyManager& value)
{
    childManagers.emplace_back(&value);
}

bool TPropertyManager::IsCustableTo(const TPropertyManager &value) const
{
    if(this == &value) return true;
    const auto& listChilds = value.childManagers;
    if(std::find(listChilds.begin(), listChilds.end(), this) != listChilds.end())
        return true;
    for(const auto& m : listChilds)
        if(IsCustableTo(*m) == true) return true;
    return false;
}

TPropertyManager::TVecPropManList TPropertyManager::ListChildManagers() const
{
    return childManagers;
}

//TPropertyClass
bool TPropertyClass::InitProperties() noexcept
{
    if (ManagerStatic().IsInit()) return true;
    PROPERTY(TString, name, Name, SetName);
    ManagerStatic().SetIsInit();
    return true;
}

TPtrPropertyClass TPropertyClass::CreateFromType(const TString &type)
{
    const TPropertyManager &man = TPropertyManager::Manager(type);
    if(man.IsValid()) return man.CreateObj();

    return TPtrPropertyClass();
}

TString TPropertyClass::TypeClass() const
{
    return Manager().Type();
}

void TPropertyClass::SetName(const TString &value)
{
    name = value;
}

TString TPropertyClass::Name() const
{
    return name;
}

size_t TPropertyClass::CountProperty() const
{
    return Manager().CountProperty();
}

int TPropertyClass::IndexProperty(const TString &nameProperty) const
{
    return Manager().IndexProperty(nameProperty);
}

TVariable TPropertyClass::ReadProperty(int index)
{
    return Manager().ReadProperty(index, this);
}

void TPropertyClass::WriteProperty(int index, const TVariable &value)
{
    Manager().WriteProperty(index, this, value);
}

TVariable TPropertyClass::ReadProperty(const TString &nameProperty)
{
    return ReadProperty(Manager().IndexProperty(nameProperty));
}

void TPropertyClass::WriteProperty(const TString &nameProperty, const TVariable &value)
{
    WriteProperty(Manager().IndexProperty(nameProperty), value);
}

int TPropertyClass::CountInArray(int index) const
{
    return Manager().ReadCountInArray(index, this);
}

int TPropertyClass::CountInArray(const TString &nameProperty) const
{
    return CountInArray(Manager().IndexProperty(nameProperty));
}

TVariable TPropertyClass::ReadFromArray(int index, int indexArray) const
{
    return Manager().ReadFromArray(index, this, indexArray);
}

TVariable TPropertyClass::ReadFromArray(const TString &nameProperty, int indexArray) const
{
    return ReadFromArray(Manager().IndexProperty(nameProperty), indexArray);
}

void TPropertyClass::AddToArray(int index, const TVariable &value)
{
    Manager().AddToArray(index, this, value);
}

void TPropertyClass::AddToArray(const TString &nameProperty, const TVariable &value)
{
    AddToArray(Manager().IndexProperty(nameProperty), value);
}

void TPropertyClass::DelFromArray(int index, const TVariable &value)
{
    Manager().DelFromArray(index, this, value);
}

void TPropertyClass::DelFromArray(const TString &nameProperty, const TVariable &value)
{
    DelFromArray(Manager().IndexProperty(nameProperty), value);
}

bool TPropertyClass::IsCustToType(const TString &value) const
{
    return IsCustToType(TPropertyManager::Manager(value));
}

bool TPropertyClass::IsCustToType(const TPropertyManager &value) const
{
    return Manager().IsCustableTo(value);
}

TVecString ListNames(const TPtrPropertyClass &value, const TString &listProp)
{
    int index = value->IndexProperty(listProp);
    TVecString res(value->CountInArray(index));
    for(size_t i = 0; i < res.size(); i++)
        res[i] = VariableToPropClass(value->ReadFromArray(index, i))->Name();
    return res;
}

TPtrPropertyClass ClonePropertyClass(const TPtrPropertyClass &value)
{
    const TPropertyManager& man = value->Manager();
    TPtrPropertyClass res = man.CreateObj();
    for(size_t i = 0; i < man.CountProperty(); i++)
    {
        const TPropInfo & info = man.Property(i);
        if (info.IsClass())
        {
            //читаем TVariable, получаем TPropClass, клонируем его, преобразуем в TVariable, записываем в проперти
            TPtrPropertyClass cl = VariableToPropClass(info.CallGet(value.get()));
            info.CallSet(res.get(), PropertyClassToVariable(ClonePropertyClass(cl)));
        }
        else if(info.IsArray())
        {
            size_t count = info.CallGetCountArray(value.get());
            for(size_t j = 0; j < count; j++)
            {
                TPtrPropertyClass it = VariableToPropClass(info.CallGetArray(value.get(), j));
                info.CallAddArray(res.get(), PropertyClassToVariable(ClonePropertyClass(it)));
            }
        }
        else
            info.CallSet(res.get(), info.CallGet(value.get()));
    }
    return res;
}
