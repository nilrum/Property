//
// Created by user on 26.08.2019.
//

#include "Serialization.h"
#include <pugixml.hpp>

REGISTER_CODES(TSerializationResult,
    TEXT_CODE(FileNotOpen, "File not opened");
    TEXT_CODE(FileNotSave, "File not saved");
    TEXT_CODE(ErrorData, "Error loadable data");
    )

class TSerializationXml : public TSerializationInterf{
public:
    virtual TString SaveTo(TPropertyClass *value) const override;
    virtual TResult LoadFrom(const TString& text, TPropertyClass *value) const override;

    virtual TResult SaveToFile(const TString& path, TPropertyClass *value) const override;
    virtual TResult LoadFromFile(const TString& path, TPropertyClass *value) const;

    virtual TResult SavePropToFile(const TString& path, TPropertyClass *value, const TPropInfo &prop) const override;
    virtual TResult LoadPropFromFile(const TString& path, TPropertyClass *value, const TPropInfo& prop) const override;

protected:
    void Save(const TPropertyClass *obj, pugi::xml_node &node, const TString &rootName) const;
    TResult Load(TPropertyClass *obj, const pugi::xml_node &node) const;

    void Save(const TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const;
    TResult Load(TPropertyClass *obj, const pugi::xml_node &node, const TPropInfo &prop, bool isLoad = false) const;

    void SaveList(const TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const;
    TResult LoadList(TPropertyClass *obj, const pugi::xml_node &node, const TPropInfo &prop, bool isLoad = false) const;
    TResult LoadListCheck(TPropertyClass *obj, const pugi::xml_node &node, const TPropInfo &prop, bool isLoad = false) const;
};


struct xml_string_writer : public pugi::xml_writer
{
    TString result;

    virtual void write(const void *data, size_t size) override
    {
        result.append(static_cast<const char *>(data), size);
    }
};

TString TSerializationXml::SaveTo(TPropertyClass *value) const
{
    pugi::xml_document xml;
    Save(value, xml, value->TypeClass());
    xml_string_writer stringWriter;
    xml.save(stringWriter);
    return stringWriter.result;
}

TResult TSerializationXml::LoadFrom(const TString &text, TPropertyClass *value) const
{
    pugi::xml_document xml;
    if(xml.load_buffer(text.c_str(), text.size()) == false) return TSerializationResult::ErrorData;
    return Load(value, xml.first_child());
}

TResult TSerializationXml::SaveToFile(const TString &path, TPropertyClass *value) const
{
    pugi::xml_document xml;
    Save(value, xml, value->TypeClass());
    return xml.save_file(path.c_str()) ? TResult() : TSerializationResult::FileNotSave;
}

TResult TSerializationXml::LoadFromFile(const TString &path, TPropertyClass *value) const
{
    pugi::xml_document xml;
    auto res = xml.load_file(path.c_str());
    if (res) return Load(value, xml.first_child());
    if (res.status == pugi::status_file_not_found)
        return TSerializationResult::FileNotOpen;
    return TSerializationResult::ErrorData;
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
        Save(obj, root, prop);
    }
}

void TSerializationXml::Save(const TPropertyClass *obj, pugi::xml_node &node, const TPropInfo &prop) const
{
    if (prop.IsArray())
    {
        SaveList(obj, node, prop);
    } else
    {
        TVariable v = prop.CallGet(obj);
        if (v.Type() == TVariableType::Ext)
        {
            TPtrPropertyClass ptr = VariableToPropClass(v);
            if (ptr) Save(ptr.get(), node, prop.Name());
        } else
        {//обычное свойство
            pugi::xml_node child = node.append_child(prop.Name().c_str());
            child.append_attribute("value") = v.ToString().c_str();
            child.append_attribute("type") = v.TypeName().c_str();
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

TResult TSerializationXml::Load(TPropertyClass *obj, const pugi::xml_node &node) const
{
    const TPropertyManager &man = obj->Manager();

    for(auto it = node.begin(); it != node.end(); it++)
    {
        pugi::xml_node child = *it;
        const TPropInfo &prop = man.FindProperty(child.name());
        TResult res = Load(obj, child, prop);
        if(res.IsHardError()) return res;
    }
    return TResult();
}

TResult TSerializationXml::Load(TPropertyClass *obj, const pugi::xml_node &node, const TPropInfo &prop, bool isLoad) const
{
    TString kind = node.attribute("kind").value();
    if (kind == "class")
    {
        if(prop.IsReadLoadable(isLoad) == false) return TResult::Cancel;//если свойство не читаемое отменяем
        TPtrPropertyClass ptr = VariableToPropClass(prop.CallGet(obj));
        if (ptr == nullptr)
        {
            if(prop.IsLoadable(isLoad) == false) return TResult::Cancel;//если свойство только для чтения отменяем
            ptr = TPropertyClass::CreateFromType(node.attribute("type").as_string());
            prop.CallSet(obj, PropertyClassToVariable(ptr));
        }
        if (ptr) return Load(ptr.get(), node);//если объект есть или создался
    }
    else if (kind == "list")
    {
        if (prop.IsReadLoadable(isLoad) == false) return TResult::Cancel;
        if(prop.IsCheckName())
            return LoadListCheck(obj, node, prop, isLoad);
        else
            return LoadList(obj, node, prop, isLoad);
    } else
    {
        if (prop.IsLoadable() == false && isLoad == false) return TResult::Cancel;
        prop.CallSet(obj, TVariable(node.attribute("value").as_string()));
        return TResult();
    }
    return TResult::Cancel;
}
TResult TSerializationXml::LoadList(TPropertyClass *obj, const pugi::xml_node &node, const TPropInfo &prop, bool isLoad) const
{
    int num = 0;
    TPtrPropertyClass ptr;
    for(auto it = node.begin(); it != node.end(); it++)
    {
        pugi::xml_node child = *it;
        int64_t count = prop.CallGetCountArray(obj);
        if (num >= count)
        {
            if(prop.IsLoadable(isLoad) == false) continue;
            ptr = TPropertyClass::CreateFromType(child.attribute("type").value());
            if(ptr) prop.CallAddArray(obj, PropertyClassToVariable(ptr));
        }
        else
            ptr = VariableToPropClass(prop.CallGetArray(obj, num));
        if (ptr == nullptr) continue;

        TResult res = Load(ptr.get(), child);
        if(res.IsHardError()) return res;
        num++;
    }
    return TResult();
}

TResult TSerializationXml::LoadListCheck(TPropertyClass *obj, const pugi::xml_node &node, const TPropInfo &prop,
                                      bool isLoad) const
{
    TPtrPropertyClass ptr;
    int count = prop.CallGetCountArray(obj);
    for (int i = 0; i < count; i++)
    {
        ptr = VariableToPropClass(prop.CallGetArray(obj, i));
        if(ptr == nullptr) continue;
        for (auto it = node.begin(); it != node.end(); it++)
        {
            pugi::xml_node child = *it;
            const pugi::char_t *name = child.first_child().attribute("value").value();
            if(name == ptr->Name())
            {
                TResult res = Load(ptr.get(), child);
                if(res.IsHardError()) return res;
                break;
            }
        }
    }
    return TResult();
}

TResult TSerializationXml::SavePropToFile(const TString &path, TPropertyClass *value, const TPropInfo &prop) const
{
    pugi::xml_document xml;
    Save(value, xml, prop);
    return xml.save_file(path.c_str()) ? TResult() : TSerializationResult::FileNotSave;
}

TResult TSerializationXml::LoadPropFromFile(const TString &path, TPropertyClass *value, const TPropInfo &prop) const
{
    pugi::xml_document xml;
    auto res = xml.load_file(path.c_str());
    if(res) return Load(value, xml.first_child(), prop, true);
    if(res == pugi::status_file_not_found) return TSerializationResult::FileNotOpen;
    else return TSerializationResult::ErrorData;
}

class TSerializationBin : public TSerializationInterf
{
public:
    virtual TString SaveTo(TPropertyClass *value) const override { return TString(); }
    virtual TResult LoadFrom(const TString& text, TPropertyClass *value) const override { return false; };

    virtual TResult SaveToFile(const TString& path, TPropertyClass *value) const override;
    virtual TResult LoadFromFile(const TString& path, TPropertyClass *value) const;

    virtual TResult SavePropToFile(const TString& path, TPropertyClass *value, const TPropInfo &prop) const override{ return false;}
    virtual TResult LoadPropFromFile(const TString& path, TPropertyClass *value, const TPropInfo& prop) const override { return false; }

protected:
    enum TKindBin : uint8_t {kbClass = static_cast<uint8_t>(TVariableType::Ext) + 1, kbList};

    struct TNode{
        TString name;
        TString type;
        TKindBin kind;
        int count;
        int64_t size;

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

TResult TSerializationBin::SaveToFile(const TString& path, TPropertyClass *value)  const
{
    std::shared_ptr<FILE> file = OpenFile(path, TOpenFileMode::Write);
    if(file == nullptr) return false;
    Save(value, file.get(), value->Name());
    return true;
}

void TSerializationBin::Save(const TPropertyClass *obj, FILE* file, const TString& rootName) const
{
    /*fpos_t posStart = 0;
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
            if (v.Type() == TVariableType::Ext)
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
    std::fseek(file, long(posStart), SEEK_SET);//переходим в начало для перезаписи
    node.WriteNode(file);
    std::fseek(file, long(posStart), SEEK_SET);//переходим в конец*/
}

void TSerializationBin::SaveList(const TPropertyClass *obj, FILE* file, const TPropInfo &prop) const
{
    /*fpos_t posStart = 0;
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
    std::fseek(file, long(posStart), SEEK_SET);//переходим в начало для перезаписи
    node.WriteNode(file);
    std::fseek(file, long(posFinish), SEEK_SET);//переходим в конец*/
}

TResult TSerializationBin::LoadFromFile(const TString& path, TPropertyClass *value) const
{
    std::shared_ptr<FILE> file = OpenFile(path);
    if(file == nullptr) return false;
    TNode node;
    node.ReadNode(file.get());
    Load(value, file.get(), node);
    return true;
}

bool TSerializationBin::Load(TPropertyClass *obj, FILE* file, const TNode& node) const
{
    const TPropertyManager &man = obj->Manager();

    for(int i = 0; i < node.count; i++)
    {
        TNode child;
        child.ReadNode(file);
        const TPropInfo &prop = man.FindProperty(child.name);
        if(prop.IsValid() == false)//если такого свойства уже нет
        {
            std::fseek(file, long(child.size), SEEK_CUR);
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
                else std::fseek(file, long(child.size), SEEK_CUR);
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
                        std::fseek(file, long(child.size), SEEK_CUR);
                }
    }
    return true;
}

void TSerializationBin::LoadList(TPropertyClass *obj, FILE* file, const TNode& node, const TPropInfo &prop) const
{
    int num = 0;
    TPtrPropertyClass ptr;
    for(int i = 0; i < node.count; i++)
    {
        int count = prop.CallGetCountArray(obj);
        TNode child;
        child.ReadNode(file);
        if (num >= count)
            ptr = TPropertyClass::CreateFromType(child.type);
        else
            ptr = VariableToPropClass(prop.CallGetArray(obj, num));
        if (ptr == nullptr)
        {
            std::fseek(file, long(child.size), SEEK_CUR);
            continue;
        }
        prop.CallAddArray(obj, PropertyClassToVariable(ptr));
        Load(ptr.get(), file, child);
        num++;
    }
}
//----------------------------------------------------------------------------------------------------------------------

TSerialization::TSerialization(TSerializationKind kind) : impl(SerFromKind(kind))
{

}

std::shared_ptr<TSerializationInterf> TSerialization::SerFromKind(TSerializationKind kind)
{
    switch (kind)
    {
        case TSerializationKind::skXml : return std::make_shared<TSerializationXml>();
        case TSerializationKind::skBin : return std::make_shared<TSerializationBin>();
        default: return std::shared_ptr<TSerializationInterf>();
    }

}

TString TSerialization::SaveTo(TPropertyClass *value) const
{
    if (impl) return impl->SaveTo(value);
    return TString();
}

TResult TSerialization::LoadFrom(const TString &text, TPropertyClass *value) const
{
    if (impl) return impl->LoadFrom(text, value);
    return false;
}

TResult TSerialization::SaveToFile(const TString &path, TPropertyClass *value) const
{
    if (impl) return impl->SaveToFile(path, value);
    return false;
}

TResult TSerialization::LoadFromFile(const TString &path, TPropertyClass *value) const
{
    if (impl) return impl->LoadFromFile(path, value);
    return false;
}

TResult TSerialization::SavePropToFile(const TString &path, TPropertyClass *value, const TPropInfo &prop) const
{
    if (impl) return impl->SavePropToFile(path, value, prop);
    return false;
}

TResult TSerialization::LoadPropFromFile(const TString &path, TPropertyClass *value, const TPropInfo &prop) const
{
    if (impl) return impl->LoadPropFromFile(path, value, prop);
    return false;
}

