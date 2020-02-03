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

class TSerialization : public TSerializationInterf
{
public:
    TSerialization();

    TString SaveTo(const TPropertyClass &value) const override;

    bool LoadFrom(TPropertyClass &value, TString text) const override;

    bool SaveToFile(TString path, const TPropertyClass &value) const override;

    bool LoadFromFile(TString path, TPropertyClass &value) const override;

protected:
    std::shared_ptr<TSerializationInterf> impl;
};


#endif //TESTAPP_SERIALIZATION_H
