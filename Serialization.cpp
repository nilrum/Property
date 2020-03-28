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

        TString kind = child.attribute("kind").value();
        if (kind == "class")
        {
            if(prop.IsReadLoadable() == false) continue;
            TPtrPropertyClass ptr = VariableToPropClass(prop.CallGet(obj));
            if (static_cast<bool>(ptr) == false)
            {
                if(prop.IsLoadable() == false) continue;
                ptr = TPropertyClass::CreateFromType(child.attribute("type").as_string());
                prop.CallSet(obj, PropertyClassToVariable(ptr));
            }
            if (ptr) Load(ptr.get(), child);//не стал проверять результат
        } else if (kind == "list")
        {
            if (prop.IsLoadable() == false) continue;
            LoadList(obj, child, prop);
        } else
        {
            if (prop.IsLoadable() == false) continue;
            prop.CallSet(obj, TVariable(child.attribute("value").as_string()));
        }
    }
    return true;
}

void TSerializationXml::LoadList(TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const
{
    int num = 0;
    TPtrPropertyClass ptr;
    for(auto it = node.begin(); it != node.end(); it++)
    {
        pugi::xml_node child = *it;
        int64_t count = prop.CallGetCountArray(obj);
        if (num >= count)
        {
            ptr = TPropertyClass::CreateFromType(child.attribute("type").value());
            if(ptr) prop.CallAddArray(obj, PropertyClassToVariable(ptr));
        }
        else
            ptr = VariableToPropClass(prop.CallGetArray(obj, num));
        if (ptr == nullptr) continue;

        Load(ptr.get(), child);
        num++;
    }
}

class TSerializationBin : public TSerializationInterf
{
public:
    TString SaveTo(const TPropertyClass &value) const override{ return TString(); };
    bool LoadFrom(TPropertyClass &value, TString text) const override{ return false; };

    bool SaveToFile(TString path, const TPropertyClass &value) const override;
    bool LoadFromFile(TString path, TPropertyClass &value) const override;

protected:
    enum TKindBin : uint8_t {kbClass = static_cast<uint8_t>(TVariableType::vtExt) + 1, kbList};

    struct TNode{
        TString name;
        TString type;
        TKindBin kind;
        size_t count;
        size_t size;

        template <typename T>
        void Write(FILE* file, T& value)
        {
            std::fwrite(&value, sizeof(T), 1, file);
        }
        void Write(FILE* file, TString& value)
        {
            uint32_t lenght = value.size();
            std::fwrite(&lenght, sizeof(uint32_t), 1, file);
            std::fwrite(&value[0], sizeof(char), lenght, file);
        }

        template <typename T>
        void Read(FILE* file, T& value)
        {
            std::fread(&value, sizeof(T), 1, file);
        }
        void Read(FILE* file, TString& value)
        {
            uint32_t lenght = 0;
            std::fread(&lenght, sizeof(uint32_t), 1, file);
            value.resize(lenght);
            std::fread(&value[0], sizeof(char), lenght, file);
        }

        void WriteNode(FILE* file)
        {
            Write(file, name);
            Write(file, type);
            Write(file, kind);
            Write(file, count);
            Write(file, size);
        }

        void ReadNode(FILE* file)
        {
            Read(file, name);
            Read(file, type);
            Read(file, kind);
            Read(file, count);
            Read(file, size);
        }

        TVariable ReadValue(FILE* file)
        {
            uint32_t lenght = 0;
            std::fread(&lenght, sizeof(uint32_t), 1, file);

            std::vector<uint8_t> buffer(lenght);
            std::fread(&buffer[0], sizeof(uint8_t), lenght, file);

            return TVariable::FromData(static_cast<TVariableType>(kind), &buffer[0], buffer.size());
        }
        void WriteValue(FILE* file, const TVariable& value)
        {
            std::vector<uint8_t> buffer = value.ToData();
            uint32_t lenght = buffer.size();
            std::fwrite(&lenght, sizeof(uint32_t), 1, file);
            std::fwrite(&buffer[0], sizeof(char), lenght, file);
        }
    };
    bool Load(TPropertyClass *obj, FILE* file, const TNode& node) const;
    void Save(const TPropertyClass *obj, FILE* file, const TString& rootName) const;

    void LoadList(TPropertyClass *obj, FILE* file, const TNode& node, const TPropInfo &prop) const;
    void SaveList(const TPropertyClass *obj, FILE* file, const TPropInfo &prop) const;
};

bool TSerializationBin::SaveToFile(TString path, const TPropertyClass &value) const
{
    std::shared_ptr<FILE> file(std::fopen(path.c_str(), "wb"), [](FILE* file){ std::fclose(file); });
    if(file == nullptr) return false;
    Save(&value, file.get(), value.Name());
    return true;
}

void TSerializationBin::Save(const TPropertyClass *obj, FILE* file, const TString& rootName) const
{
    fpos_t posStart = 0;
    std::fgetpos(file, &posStart);//сохраняем позицию в которой надо переписать
    TNode node = {rootName, obj->TypeClass(), kbClass, 0, 0};
    node.WriteNode(file);
    fpos_t posCalc = 0;
    std::fgetpos(file, &posCalc);//сохраняем позицию для расчета рамзмера

    const TPropertyManager &man = obj->Manager();
    for (size_t i = 0; i < man.CountProperty(); i++)
    {
        const TPropInfo &prop = man.Property(i);
        if (prop.IsStorable() == false) continue;

        if (prop.IsArray())
        {
            SaveList(obj, file, prop);
            node.count++;
        }
        else
        {
            TVariable v = prop.CallGet(obj);
            if (v.Type() == TVariableType::vtExt)
            {
                TPtrPropertyClass ptr = VariableToPropClass(v);
                if (ptr)
                {
                    Save(ptr.get(), file, prop.Name());
                    node.count++;
                }
            }
            else
            {//обычное свойство
                TNode child = {prop.Name(), v.TypeName(), (TKindBin)v.Type(), 1, v.Size() + sizeof(uint32_t)};//чтобы два раза не писать
                child.WriteNode(file);
                child.WriteValue(file, v);
                node.count++;
            }
        }
    }
    fpos_t posFinish = 0;
    std::fgetpos(file, &posFinish);//сохраняем позицию для расчета рамзмера
    node.size = posFinish - posCalc;
    std::fseek(file, posStart, SEEK_SET);//переходим в начало для перезаписи
    node.WriteNode(file);
    std::fseek(file, posFinish, SEEK_SET);//переходим в конец
}

void TSerializationBin::SaveList(const TPropertyClass *obj, FILE* file, const TPropInfo &prop) const
{
    fpos_t posStart = 0;
    std::fgetpos(file, &posStart);//сохраняем позицию в которой надо переписать
    TNode node = {prop.Name(), "list", kbList, 0, 0};
    node.WriteNode(file);
    fpos_t posCalc = 0;
    std::fgetpos(file, &posCalc);//сохраняем позицию для расчета рамзмера

    node.count = prop.CallGetCountArray(obj);
    for (int i = 0; i < node.count; i++)
    {
        TPtrPropertyClass ptr = VariableToPropClass(prop.CallGetArray(obj, i));
        if (ptr) Save(ptr.get(), file, prop.Name() + std::to_string(i));
    }

    fpos_t posFinish = 0;
    std::fgetpos(file, &posFinish);//сохраняем позицию для расчета рамзмера
    node.size = posFinish - posCalc;
    std::fseek(file, posStart, SEEK_SET);//переходим в начало для перезаписи
    node.WriteNode(file);
    std::fseek(file, posFinish, SEEK_SET);//переходим в конец
}

bool TSerializationBin::LoadFromFile(TString path, TPropertyClass &value) const
{
    std::shared_ptr<FILE> file(std::fopen(path.c_str(), "rb"), [](FILE* file){ std::fclose(file); });
    if(file == nullptr) return false;
    TNode node;
    node.ReadNode(file.get());
    Load(&value, file.get(), node);
    return true;
}

bool TSerializationBin::Load(TPropertyClass *obj, FILE* file, const TNode& node) const
{
    const TPropertyManager &man = obj->Manager();

    for(size_t i = 0; i < node.count; i++)
    {
        TNode child;
        child.ReadNode(file);
        const TPropInfo &prop = man.FindProperty(child.name);
        if(prop.IsValid() == false)//если такого свойства уже нет
        {
            std::fseek(file, child.size, SEEK_CUR);
        }
        else
            if (child.kind == kbClass)
            {
                if(prop.IsReadLoadable() == false) continue;
                TPtrPropertyClass ptr = VariableToPropClass(prop.CallGet(obj));
                if (ptr == nullptr)
                {
                    if(prop.IsLoadable() == false) continue;
                    ptr = TPropertyClass::CreateFromType(child.type);
                    prop.CallSet(obj, PropertyClassToVariable(ptr));
                }
                if (ptr) Load(ptr.get(), file, child);//не стал проверять результат
                else std::fseek(file, child.size, SEEK_CUR);
            }
            else
                if (child.kind == kbList)
                {
                    if (prop.IsLoadable() == false) continue;
                    LoadList(obj, file, child, prop);
                } else
                {
                    if (prop.IsLoadable())
                        prop.CallSet(obj, child.ReadValue(file));
                    else
                        std::fseek(file, child.size, SEEK_CUR);
                }
    }
    return true;
}

void TSerializationBin::LoadList(TPropertyClass *obj, FILE* file, const TNode& node, const TPropInfo &prop) const
{
    int num = 0;
    TPtrPropertyClass ptr;
    for(size_t i = 0; i < node.count; i++)
    {
        int64_t count = prop.CallGetCountArray(obj);
        TNode child;
        child.ReadNode(file);
        if (num >= count)
            ptr = TPropertyClass::CreateFromType(child.type);
        else
            ptr = VariableToPropClass(prop.CallGetArray(obj, num));
        if (ptr == nullptr)
        {
            std::fseek(file, child.size, SEEK_CUR);
            continue;
        }
        prop.CallAddArray(obj, PropertyClassToVariable(ptr));
        Load(ptr.get(), file, child);
        num++;
    }
}
//----------------------------------------------------------------------------------------------------------------------

TSerialization::TSerialization() : impl(std::make_shared<TSerializationXml>())
{

}

TSerialization::TSerialization(TSerializationKind kind) : impl(SerFromKind(kind))
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

std::shared_ptr<TSerializationInterf> TSerialization::SerFromKind(TSerializationKind kind)
{
    switch (kind)
    {
        case TSerializationKind::skXml : return std::make_shared<TSerializationXml>();
        case TSerializationKind::skBin : return std::make_shared<TSerializationBin>();
    }

}

