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

enum class TVariableType
{
    vtNone, vtInt, vtUInt, vtDouble, vtStr, vtExt
};

class TVariableExt
{
public:
    virtual std::string TypeName() const = 0;
    virtual size_t Size() const = 0;
};

template<typename T>
class TVariableExtValue : public TVariableExt
{
private:
    T value;
public:
    TVariableExtValue(const T &_value) : value(_value) {}
    TVariableExtValue(T &&_value) : value(std::move(_value)) {}

    virtual std::string TypeName() const override { return "TVariableExtValue"; }
    virtual size_t Size() const override { return sizeof(T); };

    T &Get() { return value; }
    const T &Get() const { return value; }
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

    template <typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value>>
        TVariable(T value)
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
    T ToType() const//вариант приведения когда мы точно знаем что там лежит
    {
        if (varType == TVariableType::vtExt)
        {
            TPtrVariableExtValue<T> rez = std::dynamic_pointer_cast<TVariableExtValue<T>>(std::any_cast<TPtrVariableExt>(varValue));
            if (rez) return rez->Get();
        }
        return T();
    }

private:
    std::any varValue;
    TVariableType varType;
};

#endif //TESTAPP_VARIABLE_H
