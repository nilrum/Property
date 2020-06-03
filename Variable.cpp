//
// Created by user on 22.08.2019.
//

#include "Variable.h"
#include <stdexcept>

TVariable::TVariable() : varType(TVariableType::vtNone) {}

TVariable::TVariable(const std::string &value) : varValue(value), varType(TVariableType::vtStr) {}

TVariable::TVariable(const char *value) : varValue(std::string(value)), varType(TVariableType::vtStr) {}

TVariable::TVariable(TPtrVariableExt &&value) : varValue(value), varType(TVariableType::vtExt) {}

TVariable::TVariable(const TVariable &oth) : varType(oth.varType), varValue(oth.varValue){}

TVariable &TVariable::operator=(const TVariable &oth)
{
    Assign(oth);
    return *this;
}

TVariable::TVariable(TVariable &&oth) : varType(std::move(oth.varType)), varValue(std::move(oth.varValue))
{
    oth.varType = TVariableType::vtNone;
}

TVariable &TVariable::operator=(TVariable &&oth)
{
    varType = std::move(oth.varType);
    varValue = std::move(oth.varValue);
    oth.varType = TVariableType::vtNone;
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
        case TVariableType::vtInt:
            return "int";
        case TVariableType ::vtUInt:
            return "unsigned int";
        case TVariableType::vtDouble:
            return "double";
        case TVariableType::vtStr:
            return "string";
        case TVariableType::vtEnum:
            return "enum";
        case TVariableType::vtExt:
            return std::any_cast<TEnum>(varValue).TypeEnum();
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
            case TVariableType::vtNone :
                return 0;
            case TVariableType::vtInt:
                return std::any_cast<int64_t>(varValue);
            case TVariableType::vtUInt:
                return static_cast<int64_t >(std::any_cast<uint64_t>(varValue));
            case TVariableType::vtDouble:
                return static_cast<int64_t >(std::any_cast<double>(varValue));
            case TVariableType::vtStr:
                return std::stoll(std::any_cast<std::string>(varValue));
            case TVariableType::vtEnum:
                return std::any_cast<TEnum>(varValue).Index();
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
            case TVariableType::vtNone :
                return 0;
            case TVariableType::vtInt:
                return std::any_cast<int64_t>(varValue);
            case TVariableType::vtUInt:
                return std::any_cast<uint64_t>(varValue);
            case TVariableType::vtDouble:
                return static_cast<uint64_t >(std::any_cast<double>(varValue));
            case TVariableType::vtStr:
                return std::stoull(std::any_cast<std::string>(varValue));
            case TVariableType::vtEnum:
                return std::any_cast<TEnum>(varValue).Index();
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
            case TVariableType::vtNone :
                return 0.;
            case TVariableType::vtInt:
                return static_cast<double >(std::any_cast<int64_t>(varValue));
            case TVariableType::vtUInt:
                return static_cast<double >(std::any_cast<uint64_t>(varValue));
            case TVariableType::vtDouble:
                return std::any_cast<double>(varValue);
            case TVariableType::vtStr:
                return std::stod(std::any_cast<std::string>(varValue));
            case TVariableType::vtEnum:
                return double(std::any_cast<TEnum>(varValue).Index());
        }
    }
    catch (std::invalid_argument)
    {}
    return 0;
}

bool TVariable::ToBool() const
{
    if(varType == TVariableType::vtStr)
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
        case TVariableType::vtNone :
            return std::string();
        case TVariableType::vtInt:
            return std::to_string(std::any_cast<int64_t>(varValue));
        case TVariableType::vtUInt:
            return std::to_string(std::any_cast<uint64_t>(varValue));
        case TVariableType::vtDouble:
            return std::to_string(std::any_cast<double>(varValue));
        case TVariableType::vtStr:
            return std::any_cast<std::string>(varValue);
        case TVariableType::vtEnum:
            return std::any_cast<TEnum>(varValue).Name();
        default:
            return std::string();
    }
}

TVariable TVariable::FromData(const TVariableType &type, void *data, const size_t &count)
{
    switch (type)
    {
        case TVariableType::vtEnum:
        {
            int32_t value = 0;
            memcpy(&value, data, sizeof(int32_t));
            return TVariable(value);
        }
        case TVariableType::vtInt:
        {
            int64_t value = 0;
            memcpy(&value, data, sizeof(int64_t));
            return TVariable(value);
        }
        case TVariableType::vtUInt:
        {
            uint64_t value = 0;
            memcpy(&value, data, sizeof(uint64_t));
            return TVariable(value);
        }
        case TVariableType::vtDouble:
        {
            double value = 0;
            memcpy(&value, data, sizeof(double));
            return TVariable(value);
        }
        case TVariableType::vtStr:
        {
            std::string value((char*)data, count);
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
        case TVariableType::vtNone: return 0;
        case TVariableType::vtInt: return sizeof(int64_t);
        case TVariableType::vtUInt: return sizeof(size_t);
        case TVariableType::vtDouble: return sizeof(double);
        case TVariableType::vtStr: return std::any_cast<std::string>(varValue).size();
        case TVariableType::vtEnum: return sizeof(int32_t);
        case TVariableType::vtExt: return std::any_cast<TPtrVariableExt>(varValue)->Size();
        default: return 0;
    }
}

std::vector<uint8_t> TVariable::ToData() const
{
    switch(varType)
    {
        case TVariableType::vtNone: return std::vector<uint8_t>();
        case TVariableType::vtInt:
        {
            int64_t value = ToInt();
            std::vector<uint8_t> rez(sizeof(int64_t));
            memcpy(&rez[0], &value, sizeof(int64_t));
            return rez;
        }
        case TVariableType::vtUInt:
        {
            uint64_t value = ToUInt();
            std::vector<uint8_t> rez(sizeof(uint64_t));
            memcpy(&rez[0], &value, sizeof(uint64_t));
            return rez;
        }
        case TVariableType::vtDouble:
        {
            double value = ToDouble();
            std::vector<uint8_t> rez(sizeof(double));
            memcpy(&rez[0], &value, sizeof(double));
            return rez;
        }
        case TVariableType::vtEnum:
        {
            int32_t value = static_cast<int>(ToInt());
            std::vector<uint8_t> rez(sizeof(int32_t));
            memcpy(&rez[0], &value, sizeof(int32_t));
            return rez;
        }
        case TVariableType::vtStr:
        {
            std::string value = ToString();
            std::vector<uint8_t> rez(value.size());
            memcpy(&rez[0], &value[0], value.size());
            return rez;
        }
        case TVariableType::vtExt: return std::vector<uint8_t>();

        default: return std::vector<uint8_t>();
    }
}

std::any TVariable::GetAny() const
{
    return varValue;
}

TEnum TVariable::GetEnum() const
{
    if (varType == TVariableType::vtEnum)
        return std::any_cast<TEnum>(varValue);
    return TEnum();
}

std::vector<std::string> EnumNamesFromVariable(const TVariable &value)
{
    return value.GetEnum().Names();
}

