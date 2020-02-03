//
// Created by user on 22.08.2019.
//

#include "Variable.h"
#include <stdexcept>

TVariable::TVariable() : varType(TVariableType::vtNone) {}
/*
//TVariable::TVariable(int value) : TVariable(static_cast<int64_t>(value)){}

//TVariable::TVariable(unsigned int value): TVariable(static_cast<int64_t>(value)){}

TVariable::TVariable(int64_t value) : varValue(value), varType(TVariableType::vtInt) {}
TVariable::TVariable(size_t value) : varValue(value), varType(TVariableType::vtInt) {}

TVariable::TVariable(double value) : varValue(value), varType(TVariableType::vtDouble) {}

TVariable::TVariable(bool value) : varValue(value), varType(TVariableType::vtBool) {}
*/
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
        case TVariableType::vtExt:
            return std::any_cast<TPtrVariableExt>(varValue)->TypeName();
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
                return static_cast<int64_t >(std::any_cast<size_t>(varValue));
            case TVariableType::vtDouble:
                return static_cast<int64_t >(std::any_cast<double>(varValue));
            case TVariableType::vtStr:
                return std::stoll(std::any_cast<std::string>(varValue));
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
                return static_cast<double >(std::any_cast<size_t>(varValue));
            case TVariableType::vtDouble:
                return std::any_cast<double>(varValue);
            case TVariableType::vtStr:
                return std::stod(std::any_cast<std::string>(varValue));
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
        std::string v = std::any_cast<std::string>(varValue);
        return v.size() != 0 && v != "false";
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
            return std::to_string(std::any_cast<size_t>(varValue));
        case TVariableType::vtDouble:
            return std::to_string(std::any_cast<double>(varValue));
        case TVariableType::vtStr:
            return std::any_cast<std::string>(varValue);
        default:
            return std::string();
    }
}


