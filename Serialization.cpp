//
// Created by user on 26.08.2019.
//

#include "Serialization.h"
#include <pugixml.hpp>

class TSerializationXml : public TSerializationInterf
{
public:
    TString SaveTo(const TPropertyClass &value) const override;
    bool LoadFrom(TPropertyClass &value, TString text) const override;

    bool SaveToFile(TString path, const TPropertyClass &value) const override;
    bool LoadFromFile(TString path, TPropertyClass &value) const override;

protected:
    void Save(const TPropertyClass *obj, pugi::xml_node &node, const TString &rootName) const;
    bool Load(TPropertyClass *obj, const pugi::xml_node &node) const;

    void SaveList(const TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const;
    void LoadList(TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const;
};

TSerialization::TSerialization() : impl(std::make_shared<TSerializationXml>())
{

}

TString TSerialization::SaveTo(const TPropertyClass &value) const
{
    if (impl) return impl->SaveTo(value);
    return TString();
}

bool TSerialization::LoadFrom(TPropertyClass &value, TString text) const
{
    if (impl) return impl->LoadFrom(value, text);
    return false;
}

bool TSerialization::SaveToFile(TString path, const TPropertyClass &value) const
{
    if (impl) return impl->SaveToFile(path, value);
    return false;
}

bool TSerialization::LoadFromFile(TString path, TPropertyClass &value) const
{
    if (impl) return impl->LoadFromFile(path, value);
    return false;
}

struct xml_string_writer : public pugi::xml_writer
{
    TString rezult;

    virtual void write(const void *data, size_t size) override
    {
        rezult.append(static_cast<const char *>(data), size);
    }
};

TString TSerializationXml::SaveTo(const TPropertyClass &value) const
{
    pugi::xml_document xml;
    Save(&value, xml, value.TypeClass());
    xml_string_writer stringWriter;
    xml.save(stringWriter);
    return stringWriter.rezult;
}

bool TSerializationXml::LoadFrom(TPropertyClass &value, TString text) const
{
    pugi::xml_document xml;
    if (xml.load_buffer(text.c_str(), text.size()) == false) return false;
    return Load(&value, xml.first_child());
}

bool TSerializationXml::SaveToFile(TString path, const TPropertyClass &value) const
{
    pugi::xml_document xml;
    Save(&value, xml, value.TypeClass());
    return xml.save_file(path.c_str());
}

bool TSerializationXml::LoadFromFile(TString path, TPropertyClass &value) const
{
    pugi::xml_document xml;
    if (xml.load_file(path.c_str()) == false) return false;
    return Load(&value, xml.first_child());
}

void TSerializationXml::Save(const TPropertyClass *obj, pugi::xml_node &node, const TString &rootName) const
{
    pugi::xml_node root = node.append_child(rootName.c_str());
    root.append_attribute("type") = obj->TypeClass().c_str();
    root.append_attribute("kind") = "class";
    const TPropertyManager &man = obj->Manager();
    for (size_t i = 0; i < man.CountProperty(); i++)
    {
        const TPropInfo &prop = man.Property(i);
        if (prop.IsStorable() == false) continue;

        if (prop.IsArray())
        {
            SaveList(obj, root, prop);
        } else
        {
            TVariable v = prop.CallGet(obj);
            if (v.Type() == TVariableType::vtExt)
            {
                TPtrPropertyClass ptr = VariableToPropClass(v);
                if (ptr) Save(ptr.get(), root, prop.Name());
            } else
            {//обычное свойство
                pugi::xml_node child = root.append_child(prop.Name().c_str());
                child.append_attribute("value") = v.ToString().c_str();
                child.append_attribute("type") = v.TypeName().c_str();
            }
        }
    }
}

void TSerializationXml::SaveList(const TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const
{
    pugi::xml_node list = node.append_child(prop.Name().c_str());
    list.append_attribute("kind") = "list";
    int64_t count = prop.CallGetCountArray(obj);
    for (int i = 0; i < count; i++)
    {
        TPtrPropertyClass ptr = VariableToPropClass(prop.CallGetArray(obj, i));
        if (ptr) Save(ptr.get(), list, prop.Name() + std::to_string(i));
    }
}

bool TSerializationXml::Load(TPropertyClass *obj, const pugi::xml_node &node) const
{
    const TPropertyManager &man = obj->Manager();
    //for (pugi::xml_node child : node.children())//
    for(auto it = node.begin(); it != node.end(); it++)
    {
        pugi::xml_node child = *it;
        const TPropInfo &prop = man.FindProperty(child.name());
        if (prop.IsLoadable() == false) continue;
        TString kind = child.attribute("kind").value();
        if (kind == "class")
        {
            TPtrPropertyClass ptr = VariableToPropClass(prop.CallGet(obj));
            if (static_cast<bool>(ptr) == false)
            {
                ptr = TPropertyClass::CreateFromType(child.attribute("type").as_string());
                prop.CallSet(obj, PropertyClassToVariable(ptr));
            }
            if (ptr) Load(ptr.get(), child);//не стал проверять результат
        } else if (kind == "list")
        {
            LoadList(obj, child, prop);
        } else
            prop.CallSet(obj, TVariable(child.attribute("value").as_string()));
    }
    return true;
}

void TSerializationXml::LoadList(TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const
{
    int num = 0;
    TPtrPropertyClass ptr;
    //for (pugi::xml_node child : node.children())//
    for(auto it = node.begin(); it != node.end(); it++)
    {
        pugi::xml_node child = *it;
        int64_t count = prop.CallGetCountArray(obj);
        if (num >= count)
            ptr = TPropertyClass::CreateFromType(child.attribute("type").value());
        else
            ptr = VariableToPropClass(prop.CallGetArray(obj, num));
        if (bool(ptr) == false) continue;
        Load(ptr.get(), child);
        prop.CallAddArray(obj, PropertyClassToVariable(ptr));
        num++;
    }
}