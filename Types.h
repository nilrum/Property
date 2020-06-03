//
// Created by user on 24.09.2019.
//

#ifndef TESTAPP_TYPES_H
#define TESTAPP_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <typeinfo>
#include <math.h>
#include <stdarg.h>

using TString = std::string;
using TVecString = std::vector<TString>;
using TVecDouble = std::vector<double>;
using TVecUInt = std::vector<size_t>;
using TVecInt = std::vector<int>;
using TVecBool = std::vector<bool>;
using TColor = uint64_t;

#define STR(VAL) (VAL).c_str()
template<class T>
T &Single()
{
    static T o;
    return o;
}

template<class T>
T &Single(T&& value)
{
    static T o;
    o = std::move(value);
    return o;
}

#define STATIC_ARG(TYPE, NAME, ...) static TYPE& NAME(){ static TYPE value(__VA_ARGS__); return value; };
#define STATIC(TYPE, NAME) static TYPE& NAME(){ static TYPE value; return value; };

template<typename T>
    std::vector<T> Split(const T& value, typename T::value_type delim)
{
    size_t begin = 0;
    size_t count = 0;
    std::vector<T> rez;
    for(auto i : value)
    {
        if(i == delim)
        {
            rez.emplace_back(value, begin, count);
            begin = begin + count + 1;//+1 пропускаем сам символ разделитель
            count = 0;
        }
        else
            count++;
    }
    if(count) rez.emplace_back(value, begin, count);
    return rez;
}
template<typename T>
T Trim(const T& value)
{
    auto begin = std::find_if_not(value.begin(), value.end(), ::isspace);
    auto end = value.end();
    if(begin != end)
        end = std::find_if_not(value.rbegin(), value.rend(), ::isspace).base();
    return TString(begin, end);
}

template<typename T>
std::vector<T> SplitTrim(const T& value, typename T::value_type delim)
{
    std::vector<T> rez = Split(value, delim);
    for(size_t i = 0; i < rez.size(); i++)
        rez[i] = Trim(rez[i]);
    return rez;
}

class /* [[nodiscard]]*/ TResult{
public:
    TResult():info(&typeid(int)){};
    TResult(int value):code(value), info(&typeid(int)){}
    template <typename T> TResult(const T& value):code(static_cast<int>(value)), info(&typeid(T)){}
    TResult(const TResult& oth):code(oth.code), info(oth.info){};
    void operator =(const TResult& oth){ code = oth.code; info = oth.info; }

    enum TDefault{Cancel = -1, Ok = 0};

    inline bool IsNoError() const { return code == Ok; }
    inline bool IsError() const { return code != Ok; }
    inline bool IsCancel() const { return code == Cancel; }
    template<typename T>
        inline bool Is(const T& value) { return static_cast<T>(code) == value && *info == typeid(T); }

    inline int Code() const { return code; }
    inline const std::type_info& Info() const { return *info; };

    static bool Register(const std::type_info& info, int code, const TString& text)
    {
       Enums()[info.name()][code] = text;
       return true;
    }

    static TString TextError(const TResult& value)
    {
        TMapEnums::iterator it = Enums().find(value.Info().name());
        if(it != Enums().end())
        {
            TMapCodes &codes = it->second;
            TMapCodes::iterator code = codes.find(value.Code());
            if (code != codes.end()) return code->second;
        }
        return std::to_string(value.Code());
    }

    using TMapCodes = std::map<int, TString>;
    using TMapEnums = std::map<TString, TMapCodes>;

    static TMapEnums& Enums() { static TMapEnums enums; return enums; }
private:
    int code = Ok;
    const std::type_info* info;

};


#define REGISTER_CODES(TYPE, CODE, TEXT)\
    namespace{\
        const bool r##CODE = TResult::Register(typeid(TYPE), static_cast<int>(TYPE::CODE), TEXT);\
    }


template<typename TVec>
    bool RemoveVal(TVec& vec, const typename TVec::value_type& val)
{
        typename TVec::iterator it = std::find(vec.begin(), vec.end(), val);
        if(it == vec.end()) return false;
        vec.erase(it);
        return true;
}

template<typename TVec>
bool RemoveValFor(TVec& vec, const typename TVec::value_type& val)
{
    for(auto it = vec.begin(); it != vec.end(); it++)
        if(*it == val)
        {
            vec.erase(it);
            return true;
        }
    return false;
}

template<class T>
class TPtrVector{
private:
    std::vector<T*> data;
public:
    using value_type = T;
    using iterator = typename std::vector<T*>::iterator;
    ~TPtrVector()
    {
        clear();
    }

    T& operator[] (int index){ return *data[index]; }
    const T& operator[] (int index) const { return *data[index]; }
    size_t size() const { return data.size(); }

    void push_back(T* value)
    {
        data.push_back(value);
    }
    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        data.push_back(new T(args...));
    }
    void clear()
    {
        for(size_t i = 0; i < data.size(); i++)
            delete data[i];
        data.clear();
    }

    iterator begin()
    {
        return data.begin();
    }

    iterator end()
    {
        return data.end();
    }

    void erase(iterator value)
    {
        T* val = *value;
        data.erase(value);
        delete val;
    }
    void erase(iterator b, iterator e)
    {
        for(iterator it = b; it != e; it++)
            delete (*it);
        data.erase(b, e);
    }
};

struct TDoubleCheck{
    static bool Equal(const double& one, const double& two)
    {
        return fabs(one - two) < 0.00001;
    }

    static bool Less(const double& one, const double& two)
    {
        return two - one > 0.00001;
    }

    static bool LessEq(const double& one, const double& two)
    {
        return Less(one, two) || Equal(one, two);
    }

    static bool Great(const double& one, const double& two)
    {
        return one - two > 0.00001;
    }

    static bool GreatEq(const double& one, const double& two)
    {
        return Great(one, two) || Equal(one, two);
    }

    struct TDoubleLess// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return Less(x, y); }
    };

    struct TDoubleLessEq// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return LessEq(x, y); }
    };

    struct TDoubleGreater// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return Great(x, y); }
    };
    struct TDoubleGreaterEq// : public std::binary_function<double, double, bool>
    {
        bool operator()(const double& x, const double& y) const
        { return GreatEq(x, y); }
    };
};

#define STDFORMAT(FRMT, ...) StdFormat(std::snprintf(nullptr, 0, FRMT, __VA_ARGS__) + 1, FRMT, __VA_ARGS__)


inline std::string StdFormat(int sizeBuf, const char* frmt,  ...)
{
    va_list args;
    va_start(args, frmt);
    std::vector<char> buf(sizeBuf);
    std::vsnprintf(&buf[0], buf.size(), frmt, args);
    return &buf[0];
}

class TFormatDouble{
private:
    int maxEndCount;//максимум три знака после запятой
    int maxBegCount;//максимум пять знаков перед запятой
    int beginCheck;//для 5ти знаков будет максимальное 1000000
    int endCheck;
    bool isUseUtf8 = true;
    static constexpr uint8_t textPows[8][8] = {
            {0xC3, 0x97, '1', '0', 0xC2, 0xB2, 0},        // * 10 ^ 2
            {0xC3, 0x97, '1', '0', 0xC2, 0xB3, 0},        // 3
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB4, 0},  //4
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB5, 0},  //5
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB6, 0},  //6
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB7, 0},  //7
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB8, 0},  //8
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB9, 0}  //9
            };
public:
    TFormatDouble(int maxEnd = 3, int maxBeg = 6){ Set(maxEnd, maxBeg); }

    void Set(int maxEnd, int maxBeg = 6)
    {
        maxEndCount = maxEnd;
        maxBegCount = std::max(2, maxBeg);

        beginCheck = std::pow(10, maxBegCount);
        endCheck = std::pow(10, maxEndCount);
    }
    TString Format( double value) const
    {
        if(std::isnan(value))
            return "nan";
        if(value < beginCheck)
        {
            int begVal = value;
            int endVal = (value - begVal + 0.0000001) * endCheck;

            if(endVal)
            {
                int endCheckVal = endCheck;
                while (endCheckVal > 1)
                {
                    endCheckVal = endCheckVal / 10;
                    if ((endVal / endCheckVal) % 10 < 5)
                    {
                        endCheckVal = endCheckVal * 10;
                        break;
                    }
                }
                endVal = endVal / endCheckVal;
            }

            if(endVal == 0)
                return std::to_string(begVal);
            else
                return std::to_string(begVal) + "." + std::to_string(endVal);
        }
        else
        {
            double begin = beginCheck;
            double val = value / begin;
            int pow = maxBegCount;
            while (val >= 10)
            {
                pow++;
                begin = begin * 10;
                val = value / begin;
            }
            if(isUseUtf8 && pow < 10)
                return Format(val) + TString((char*)textPows[pow - 2]);
            else
                return Format(val) + "*10^" + std::to_string(pow);
        }
    }

    static TString Format(double value, int maxEnd, int maxBeg = 6)
    {
        return TFormatDouble(maxEnd, maxBeg).Format(value);
    }

};


#endif //TESTAPP_TYPES_H
