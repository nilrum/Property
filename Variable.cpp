//
// Created by user on 22.08.2019.
//

#include "Variable.h"
#include <stdexcept>
#include <cstring>

TVariable::TVariable() : varType(TVariableType::None) {}

TVariable::TVariable(const std::string &value) : varValue(value), varType(TVariableType::Str) {}

TVariable::TVariable(const char *value) : varValue(std::string(value)), varType(TVariableType::Str) {}

TVariable::TVariable(TPtrVariableExt &&value) : varValue(value), varType(TVariableType::Ext) {}

TVariable::TVariable(const TVariable &oth) : varType(oth.varType), varValue(oth.varValue){}

TVariable &TVariable::operator=(const TVariable &oth)
{
    Assign(oth);
    return *this;
}

TVariable::TVariable(TVariable &&oth) : varType(std::move(oth.varType)), varValue(std::move(oth.varValue))
{
    oth.varType = TVariableType::None;
}

TVariable &TVariable::operator=(TVariable &&oth)
{
    varType = std::move(oth.varType);
    varValue = std::move(oth.varValue);
    oth.varType = TVariableType::None;
    return *this;
}

void TVariable::Assign(const TVariable &oth)
{
    varType = oth.varType;
    varValue = oth.varValue;
}


TVariableType TVariable::Type() const
{
    return varType;
}

std::string TVariable::TypeName() const
{
    switch (varType)
    {
        case TVariableType::Int:
            return "int";
        case TVariableType ::UInt:
            return "unsigned int";
        case TVariableType::Double:
            return "double";
        case TVariableType::Str:
            return "string";
        case TVariableType::Enum:
            return std::any_cast<TEnum>(varValue).TypeEnum();
        case TVariableType::Bool:
            return "bool";
        default:
            return std::string();
    }
}


int64_t TVariable::ToInt() const
{
    try
    {
        switch (varType)
        {
            case TVariableType::None :
                return 0;
            case TVariableType::Int:
                return std::any_cast<int64_t>(varValue);
            case TVariableType::UInt:
                return static_cast<int64_t >(std::any_cast<uint64_t>(varValue));
            case TVariableType::Double:
                return static_cast<int64_t >(std::any_cast<double>(varValue));
            case TVariableType::Str:
                return std::stoll(std::any_cast<std::string>(varValue));
            case TVariableType::Enum:
                return std::any_cast<TEnum>(varValue).Index();
            case TVariableType::Bool:
                return std::any_cast<bool>(varValue);
        }
    }
    catch (std::invalid_argument)
    {}
    return 0;
}

uint64_t TVariable::ToUInt() const
{
    try
    {
        switch (varType)
        {
            case TVariableType::None :
                return 0;
            case TVariableType::Int:
                return std::any_cast<int64_t>(varValue);
            case TVariableType::UInt:
                return std::any_cast<uint64_t>(varValue);
            case TVariableType::Double:
                return static_cast<uint64_t >(std::any_cast<double>(varValue));
            case TVariableType::Str:
                return std::stoull(std::any_cast<std::string>(varValue));
            case TVariableType::Enum:
                return std::any_cast<TEnum>(varValue).Index();
            case TVariableType::Bool:
                return std::any_cast<bool>(varValue);
        }
    }
    catch (std::invalid_argument)
    {}
    return 0;
}

double TVariable::ToDouble() const
{
    try
    {
        switch (varType)
        {
            case TVariableType::None :
                return 0.;
            case TVariableType::Int:
                return static_cast<double >(std::any_cast<int64_t>(varValue));
            case TVariableType::UInt:
                return static_cast<double >(std::any_cast<uint64_t>(varValue));
            case TVariableType::Double:
                return std::any_cast<double>(varValue);
            case TVariableType::Str:
                return std::stod(std::any_cast<std::string>(varValue));
            case TVariableType::Enum:
                return double(std::any_cast<TEnum>(varValue).Index());
            case TVariableType::Bool:
                return std::any_cast<bool>(varValue);
        }
    }
    catch (std::invalid_argument)
    {}
    return 0;
}

bool TVariable::ToBool() const
{
    if(varType == TVariableType::Str)
    {
        auto v = std::any_cast<std::string>(varValue);
        return v.empty() == false && v != "false" && v != "0";
    }
    else
        return ToDouble() != 0.0;
}

std::string TVariable::ToString() const
{
    switch (varType)
    {
        case TVariableType::None :
            return std::string();
        case TVariableType::Int:
            return std::to_string(std::any_cast<int64_t>(varValue));
        case TVariableType::UInt:
            return std::to_string(std::any_cast<uint64_t>(varValue));
        case TVariableType::Double:
            return std::to_string(std::any_cast<double>(varValue));
        case TVariableType::Str:
            return std::any_cast<std::string>(varValue);
        case TVariableType::Enum:
            return std::any_cast<TEnum>(varValue).Name();
        case TVariableType::Bool:
            return ToBool() ? "true" : "false";
        default:
            return std::string();
    }
}

TVariable TVariable::FromData(const TVariableType &type, void *data, const size_t &count)
{
    switch (type)
    {
        case TVariableType::Enum:
        {
            int32_t value = 0;
            memcpy(&value, data, sizeof(int32_t));
            return TVariable(value);
        }
        case TVariableType::Int:
        {
            int64_t value = 0;
            memcpy(&value, data, sizeof(int64_t));
            return TVariable(value);
        }
        case TVariableType::UInt:
        {
            uint64_t value = 0;
            memcpy(&value, data, sizeof(uint64_t));
            return TVariable(value);
        }
        case TVariableType::Double:
        {
            double value = 0;
            memcpy(&value, data, sizeof(double));
            return TVariable(value);
        }
        case TVariableType::Str:
        {
            std::string value((char*)data, count);
            return TVariable(value);
        }

        case TVariableType::Bool:
        {
            bool value = false;
            memcpy(&value, data, sizeof(bool));
            return TVariable(value);
        }
        default:
            return TVariable();
    }
}

size_t TVariable::Size() const
{
    switch(varType)
    {
        case TVariableType::None: return 0;
        case TVariableType::Int: return sizeof(int64_t);
        case TVariableType::UInt: return sizeof(size_t);
        case TVariableType::Double: return sizeof(double);
        case TVariableType::Str: return std::any_cast<std::string>(varValue).size();
        case TVariableType::Enum: return sizeof(int32_t);
        case TVariableType::Ext: return std::any_cast<TPtrVariableExt>(varValue)->Size();
        case TVariableType::Bool: return sizeof(bool);
        default: return 0;
    }
}

std::vector<uint8_t> TVariable::ToData() const
{
    switch(varType)
    {
        case TVariableType::None: return std::vector<uint8_t>();
        case TVariableType::Int:
        {
            int64_t value = ToInt();
            std::vector<uint8_t> res(sizeof(int64_t));
            memcpy(&res[0], &value, sizeof(int64_t));
            return res;
        }
        case TVariableType::UInt:
        {
            uint64_t value = ToUInt();
            std::vector<uint8_t> res(sizeof(uint64_t));
            memcpy(&res[0], &value, sizeof(uint64_t));
            return res;
        }
        case TVariableType::Double:
        {
            double value = ToDouble();
            std::vector<uint8_t> res(sizeof(double));
            memcpy(&res[0], &value, sizeof(double));
            return res;
        }
        case TVariableType::Enum:
        {
            int32_t value = static_cast<int>(ToInt());
            std::vector<uint8_t> res(sizeof(int32_t));
            memcpy(&res[0], &value, sizeof(int32_t));
            return res;
        }
        case TVariableType::Str:
        {
            std::string value = ToString();
            std::vector<uint8_t> res(value.size());
            memcpy(&res[0], &value[0], value.size());
            return res;
        }

        case TVariableType::Bool:
        {
            std::vector<uint8_t> res(sizeof(bool));
            res[0] = ToBool();
            return res;
        }

        case TVariableType::Ext: return std::vector<uint8_t>();

        default: return std::vector<uint8_t>();
    }
}

std::any TVariable::GetAny() const
{
    return varValue;
}

TEnum TVariable::GetEnum() const
{
    if (varType == TVariableType::Enum)
        return std::any_cast<TEnum>(varValue);
    return TEnum();
}

std::vector<std::string> EnumNamesFromVariable(const TVariable &value)
{
    return value.GetEnum().Names();
}

