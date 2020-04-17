//
// Created by user on 26.08.2019.
//

#ifndef TESTAPP_SERIALIZATION_H
#define TESTAPP_SERIALIZATION_H

#include "PropertyClass.h"

class TSerializationInterf
{
public:
    virtual TString SaveTo(const TPropertyClass& value) const = 0;
    virtual bool LoadFrom(const TString& text, TPropertyClass& value) const = 0;

    virtual bool SaveToFile( const TString& path, const TPropertyClass& value) const = 0;
    virtual bool LoadFromFile(const TString& path, TPropertyClass& value) const = 0;

    virtual bool SavePropToFile(const TString& path, const TPropertyClass& value, const TPropInfo &prop) const = 0;
    virtual bool LoadPropFromFile(const TString& path, TPropertyClass& value, const TPropInfo& prop) const = 0;
};

enum class TSerializationKind{skXml = 0, skBin, skCount};

class TSerialization : public TSerializationInterf{
public:
    TSerialization(TSerializationKind kind = TSerializationKind::skXml);

    virtual TString SaveTo(const TPropertyClass& value) const override;
    virtual bool LoadFrom(const TString& text, TPropertyClass& value) const override;

    virtual bool SaveToFile( const TString& path, const TPropertyClass& value) const override;
    virtual bool LoadFromFile(const TString& path, TPropertyClass& value) const;

    virtual bool SavePropToFile(const TString& path, const TPropertyClass& value, const TPropInfo &prop) const override;
    virtual bool LoadPropFromFile(const TString& path, TPropertyClass& value, const TPropInfo& prop) const override;

    inline bool SavePropToFileName(const TString& path, const TPropertyClass& value, const TString& name) const;
    inline bool LoadPropFromFileName(const TString& path, TPropertyClass& value, const TString& name) const;
protected:
    std::shared_ptr<TSerializationInterf> impl;
    static std::shared_ptr<TSerializationInterf> SerFromKind(TSerializationKind kind);
};

bool TSerialization::SavePropToFileName(const TString &path, const TPropertyClass &value, const TString &name) const
{
    const TPropInfo & prop = value.Manager().FindProperty(name);
    if(prop.IsValid() == false) return false;
    return SavePropToFile(path, value, prop);
}

bool TSerialization::LoadPropFromFileName(const TString &path, TPropertyClass &value, const TString &name) const
{
    const TPropInfo & prop = value.Manager().FindProperty(name);
    if(prop.IsValid() == false) return false;
    return LoadPropFromFile(path, value, prop);
}


#endif //TESTAPP_SERIALIZATION_H
