//
// Created by user on 22.08.2019.
//

#ifndef TESTAPP_VARIABLE_H
#define TESTAPP_VARIABLE_H


#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <any>
#include "EnumInfo.h"

enum class TVariableType
{
    vtNone, vtInt, vtUInt, vtDouble, vtStr, vtEnum, vtExt
};

class TVariableExt{
public:
    ~TVariableExt() = default;
    virtual std::string TypeName() const = 0;
    virtual size_t Size() const = 0;

    virtual int64_t ToInt() const { return 0; }
    virtual std::string ToString() const { return std::string(); }
};

template<typename T>
class TVariableExtValue : public TVariableExt{
protected:
    T value;
public:
    TVariableExtValue(const T &_value) : value(_value) {}
    TVariableExtValue(T &&_value) : value(std::move(_value)) {}

    virtual std::string TypeName() const override { return "TVariableExtValue"; }
    virtual size_t Size() const override { return sizeof(T); };

    T &Get() { return value; }
    const T &Get() const { return value; }
};

class TEnumInterf{
public:
    virtual std::vector<std::string> EnumNames() const { return std::vector<std::string>(); };
    virtual size_t EnumNamesCount() const { return 0; }
};

template <typename T>
class TVariableExtEnum : public TVariableExtValue<T>, public TEnumInterf{
public:
    TVariableExtEnum(const T &_value):TVariableExtValue<T>(_value){}
    TVariableExtEnum(T &&_value):TVariableExtValue<T>(std::move(_value)){}
#ifdef IS_MAGIC_USED
    void FromInt(int32_t val) { TVariableExtValue<T>::value = magic_enum::enum_cast<T>(val); };
    void FromString(const std::string& str){ TVariableExtValue<T>::value = magic_enum::enum_cast<T>(str);}

    std::string ToString() const { return std::string(magic_enum::enum_name(TVariableExtValue<T>::value)); }
    int64_t ToInt() const { return static_cast<int64_t>(TVariableExtValue<T>::value); }

    std::vector<std::string> EnumNames() const
    {
        auto r = magic_enum::enum_names<T>();
        std::vector<std::string> rez;
        for(auto b = r.begin(); b != r.end(); b++)
            rez.push_back(std::string(*b));
        return rez;
    }
    size_t EnumNamesCount() const { return magic_enum::enum_count<T>(); }
#else
    static TVariableExtValue<T> FromInt(int32_t val) { return TVariableExtValue<T>(static_cast<T>(val)); };
    static TVariableExtValue<T> FromString(const std::string& str)
    {
        size_t val = std::atoi(str.c_str());

        const TVecString& vec = TEnum::EnumItems(typeid(T));
        for(size_t i = 0; i < vec.size(); i++)
            if(vec[i] == str) return TVariableExtValue<T>(static_cast<T>(i));

        return TVariableExtValue<T>(static_cast<T>(val));
    }

    virtual std::string ToString() const override
    {
        const TVecString &vec = TEnum::EnumItems(typeid(T));
        int32_t v = ToInt();
        if (v >= 0 && v < vec.size()) return vec[v];
        return std::string();
    }

    virtual int64_t ToInt() const override { return static_cast<int64_t>(TVariableExtValue<T>::value); }

    virtual std::vector<std::string> EnumNames() const override
    {
        return TEnum::EnumItems(typeid(T));
    }
    virtual size_t EnumNamesCount() const override { return TEnum::EnumItems(typeid(T)).size(); }
#endif
};

using TPtrVariableExt = std::shared_ptr<TVariableExt>;
template<typename T> using TPtrVariableExtValue = std::shared_ptr<TVariableExtValue<T>>;

class TVariable
{
public:
    TVariable();

    TVariable(const std::string &value);
    TVariable(const char *value);
    TVariable(TPtrVariableExt &&value);

    template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || std::is_enum<T>::value>>
        TVariable(T value)
        {
            if constexpr (std::is_enum<T>::value == false)
            {
                if constexpr (std::is_integral<T>::value)
                {
                    if constexpr (std::is_signed<T>::value)
                    {
                        varValue = static_cast<int64_t >(value);
                        varType = TVariableType::vtInt;
                    }
                    else
                    {
                        varValue = static_cast<uint64_t>(value);
                        varType = TVariableType::vtUInt;
                    }
                }
                else
                {
                    varValue = static_cast<double>(value);
                    varType = TVariableType::vtDouble;
                }
            }
            else
            {
                TPtrVariableExt v = std::make_shared<TVariableExtEnum<T>>(value);
                varValue = v;
                varType = TVariableType::vtEnum;
            }
        }

    TVariable(const TVariable &oth);
    TVariable(TVariable &&oth);

    TVariable &operator=(const TVariable &oth);
    void Assign(const TVariable &oth);

    TVariable &operator=(TVariable &&oth);

    TVariableType Type() const;
    size_t Size() const;

    std::string TypeName() const;

    int64_t ToInt() const;
    uint64_t ToUInt() const;
    double ToDouble() const;
    bool ToBool() const;
    std::string ToString() const;
    template <typename T>
        T ToEnum() const;

    std::vector<uint8_t> ToData() const;

    operator int64_t() const { return ToInt(); }
    operator uint64_t() const { return ToUInt(); }
    operator int32_t() const { return static_cast<int32_t>(ToInt()); }
    operator uint32_t() const { return static_cast<uint32_t>(ToUInt()); }
    operator double() const { return ToDouble(); }
    operator bool() const { return ToBool(); }
    operator std::string() const { return ToString(); }

    static TVariable FromData(const TVariableType& type, void* data, const size_t& count);
    template<typename T>
        T ToType() const;//вариант приведения когда мы точно знаем что там лежит

    TPtrVariableExt GetExt() const ;
private:
    std::any varValue;
    TVariableType varType;
};

template <typename T>
T TVariable::ToEnum() const
{
    if(varType == TVariableType::vtStr)
        return TVariableExtEnum<T>::FromString(ToString()).Get();
    return static_cast<T>(ToInt());
}

template<typename T>
T TVariable::ToType() const//вариант приведения когда мы точно знаем что там лежит
{
    if (varType == TVariableType::vtExt)
    {
        TPtrVariableExtValue<T> rez = std::dynamic_pointer_cast<TVariableExtValue<T>>(std::any_cast<TPtrVariableExt>(varValue));
        if (rez) return rez->Get();
    }
    return T();
}
std::vector<std::string> EnumNamesFromVariable(const TVariable& value);
#endif //TESTAPP_VARIABLE_H
