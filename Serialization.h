//
// Created by user on 26.08.2019.
//

#ifndef TESTAPP_SERIALIZATION_H
#define TESTAPP_SERIALIZATION_H

#include "PropertyClass.h"

class TSerializationInterf
{
public:
    virtual TString SaveTo(const TPropertyClass &value) const = 0;

    virtual bool LoadFrom(TPropertyClass &value, TString text) const = 0;

    virtual bool SaveToFile(TString path, const TPropertyClass &value) const = 0;

    virtual bool LoadFromFile(TString path, TPropertyClass &value) const = 0;
};

enum class TSerializationKind{skXml = 0, skBin, skCount};

class TSerialization : public TSerializationInterf
{
public:
    TSerialization();
    TSerialization(TSerializationKind kind);

    TString SaveTo(const TPropertyClass &value) const override;

    bool LoadFrom(TPropertyClass &value, TString text) const override;

    bool SaveToFile(TString path, const TPropertyClass &value) const override;

    bool LoadFromFile(TString path, TPropertyClass &value) const override;

protected:
    std::shared_ptr<TSerializationInterf> impl;
    static std::shared_ptr<TSerializationInterf> SerFromKind(TSerializationKind kind);
};


#endif //TESTAPP_SERIALIZATION_H
