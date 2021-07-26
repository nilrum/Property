//
// Created by user on 24.09.2019.
//

#ifndef TESTAPP_TYPES_H
#define TESTAPP_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <typeinfo>
#include <cmath>
#include <math.h>
#include <stdarg.h>
#include <memory>
#include "float.h"

using TString = std::string;
using TVecString = std::vector<TString>;
using TVecDouble = std::vector<double>;
using TVecVecDouble = std::vector<TVecDouble>;
using TVecUInt = std::vector<size_t>;
using TVecInt = std::vector<int>;
using TVecBool = std::vector<bool>;
using TVecUChar = std::vector<unsigned char>;
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

#define PROXY(FUNC) \
    [this](auto && ...a) { FUNC(std::forward<decltype(a)>(a)...); }

#define PROXY_C(FUNC, LBD_ARGS) \
    [LBD_ARGS](auto && ...a) { FUNC(); }

#define INIT_SECTION(NAME, INIT) \
    namespace INIT_##NAME{       \
        bool Fun##NAME()         \
        {                        \
            INIT\
            return true;         \
        }                        \
        const bool Cnst_##NAME = Fun##NAME();\
    }

#define CLASS_PTRS_TYPE(NAME, TYPE) \
    using TPtr##NAME = std::shared_ptr<TYPE>; \
    using TUPtr##NAME = std::unique_ptr<TYPE>;\
    using TWPtr##NAME = std::weak_ptr<TYPE>;  \
    using TRaw##NAME = TYPE*;

#define CLASS_PTRS(NAME) CLASS_PTRS_TYPE(NAME, T##NAME)

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

TString Merge(const TVecString& values, typename TString::value_type delim);

template<typename T>
T Trim(const T& value)
{
    auto isspace = [](char ch) { return std::isspace(static_cast<unsigned char>(ch)); };
    auto begin = std::find_if_not(value.begin(), value.end(), isspace);
    auto end = value.end();
    if(begin != end)
        end = std::find_if_not(value.rbegin(), value.rend(), isspace).base();
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

template<typename T>
T ToUpperCase(const T& value)
{
    T res = value;
    std::transform(value.begin(), value.end(), res.begin(), ::toupper);
    return res;
}

template<typename T>
T ToLowerCase(const T& value)
{
    T res = value;
    std::transform(value.begin(), value.end(), res.begin(), ::tolower);
    return res;
}

template<typename T>
T TrimBefore(const T& value, typename T::value_type delim, size_t maxCount = 0)
{
    if(value.empty()) return value;
    auto v = std::find(value.rbegin(), value.rend(), delim);
    if(v != value.rend())
        return T{".."} + T{value.begin() + (value.rend() - v - 1), value.end()};//-1 чтобы знак тоже входил
    if(maxCount == 0)
        return value;
    return T{value.begin() + value.size() - maxCount, value.end()};
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
    inline bool IsHardError() const { return IsError() && IsCancel() == false; }
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


#define REGISTER_CODE(TYPE, CODE, TEXT)\
    namespace{\
        const bool r##CODE = TResult::Register(typeid(TYPE), static_cast<int>(TYPE::CODE), TEXT);\
    }
#define REGISTER_CODES(TYPE, ...)       \
    namespace Namespace##TYPE{          \
        using TResType = TYPE;          \
        bool Fun##TYPE()                \
        {                               \
            __VA_ARGS__                 \
            return true;                \
        }                               \
        const bool r##TYPE = Fun##TYPE();\
    }
#define TEXT_CODE(CODE, TEXT)\
    TResult::Register(typeid(TResType), static_cast<int>(TResType::CODE), TEXT);

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
    using TVectData = std::vector<T*>;

    std::vector<T*> data;
    struct TIterator{
        typename TVectData::iterator data;
        T& operator*() const { return **data; }
        bool operator != (const TIterator& oth) const { return data != oth.data; }
        void operator ++() { data++; }
        T* operator ->() const{ return get(); }
        T* get() const { return *data; }
    };
public:
    using value_type = T;
    using iterator = TIterator;

    ~TPtrVector()
    {
        clear();
    }

    inline T& operator[] (int index){ return *data[index]; }
    inline const T& operator[] (int index) const { return *data[index]; }
    inline size_t size() const { return data.size(); }
    inline T& front() const { return *data.front(); }
    inline T& back() const { return *data.back(); }

    void push_back(const T& value)
    {
        push_back(new T(value));
    }
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
        return iterator{ data.begin() };
    }

    iterator end()
    {
        return iterator { data.end() };
    }

    iterator insert(iterator it, const T& value)
    {
        return iterator{ data.insert(it.data, new T(value)) };
    }
    void erase(iterator value)
    {
        delete value.get();
        data.erase(value.data);
    }
    void erase(iterator b, iterator e)
    {
        for(iterator it = b; it != e; it++)
            delete it.get();
        data.erase(b.data, e.data);
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

inline const char* PtrText(const char* value) { return value; }
inline const char* PtrText(const TString& value) { return STR(value); }

#define STDFORMAT(FRMT, ...) StdFormat(std::snprintf(nullptr, 0, PtrText(FRMT), __VA_ARGS__) + 1, PtrText(FRMT), __VA_ARGS__)

TString Transliteration(TString utf8text);

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
    int maxEndCount;//максимум знаков после запятой
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
            {0xC3, 0x97, '1', '0', 0xE2, 0x81, 0xB9, 0}   //9
            };
public:
    TFormatDouble(int maxEnd = 3, int maxBeg = 6){ Set(maxEnd, maxBeg); }

    void Set(int maxEnd, int maxBeg = 6)
    {
        maxEndCount = maxEnd;
        maxBegCount = std::max(2, maxBeg);

        beginCheck = int(std::pow(10, maxBegCount));
        endCheck = int(std::pow(10, maxEndCount));
    }
    TString Format(double value, double eps)
    {
        if(eps >= 1) return Round(value);
        int begVal = int(value);
        int endVal = int((value - begVal + 0.0000001) * endCheck);
        int chk = 10;
        int count = maxEndCount;
        while(endVal % chk == 0 && count > 0)
        {
            count--;
            chk *= 10;
        }
        return STDFORMAT("%.*f", count, value);
    }

    TString Round(double value) const
    {
        if(std::isnan(value))
            return "nan";
        if(value < beginCheck)
        {
            int begVal = int(value);
            int endVal = int((value - begVal + 0.0000001) * endCheck);

            if(endVal)
            {
                int endCheckVal = endCheck;
                while (endCheckVal > 1)
                {
                    endCheckVal = endCheckVal / 10;
                    /*if ((endVal / endCheckVal) % 10 < 5)
                    {
                        endCheckVal = endCheckVal * 10;
                        break;
                    }*/
                }
                endVal = endVal / endCheckVal;
            }

            if(endVal == 0)
                return std::to_string(begVal);
            else
                return std::to_string(begVal) + "." + std::to_string(std::abs(endVal));
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
                return Round(val) + TString((char*)textPows[pow - 2]);
            else
                return Round(val) + "*10^" + std::to_string(pow);
        }
    }

    static TString FormatDouble(double value, int maxEnd, int maxBeg = 6)
    {
        return TFormatDouble(maxEnd, maxBeg).Round(value);
    }

};

inline bool TryStrToInt(const TString& str, int& value)
{
    try{
        value = std::stoi(str);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

template <typename Key, typename Value, std::size_t Size>
struct TConstExprMap{
    using TMapData = std::array<std::pair<Key, Value>, Size>;
    TMapData data;
    constexpr TConstExprMap(const TMapData& value):data(value){}
    constexpr Value at(const Key& key) const{
        const auto itr = std::find_if(data.begin(), data.end(),
                                      [&key](const auto& p){ return p.first == key; });
        if(itr != data.end())
            return itr->second;
        return Value();
    }
};

enum class TOpenFileMode{ Read, Write, Append };
using TPtrFile = std::shared_ptr<FILE>;

enum class TFileResult{Ok, ErrorRead, ErrorWrite};

TPtrFile OpenFile(const std::string& path, TOpenFileMode mode = TOpenFileMode::Read);
template<typename T>
    TResult WriteFile(const T& value, const TPtrFile& file)
    {
        return std::fwrite(&value, sizeof(T), 1, file.get()) == 1 ? TResult() : TFileResult::ErrorWrite;
    }

template<typename T>
    TResult WriteFile(const T& value, size_t index, size_t count, const TPtrFile& file)
    {
        return std::fwrite(&value[index], sizeof(value[index]), count, file.get()) == count ? TResult() : TFileResult::ErrorWrite;
    }

    TResult WriteStringInFile(const TString& value, const TPtrFile& file);

template<typename T>
    TResult ReadFile(T& value, const TPtrFile& file)
    {
        return std::fread(&value, sizeof(T), 1, file.get()) == 1 ? TResult() : TFileResult::ErrorRead;
    }

template<typename T>
    TResult ReadFile(T& value, size_t index, size_t count, const TPtrFile& file)
    {
        return std::fread(&value[index], sizeof(value[index]), count, file.get()) == 1 ? TResult() : TFileResult::ErrorRead;
    }

TResult ReadStringFromFile(TString& value, const TPtrFile& file);

std::wstring WStringFromUtf8(const TString &value);

class TSimpleLog{
public:
    void Clear();
    void Log(const TString& text);

    STATIC_ARG(TString, DefaultPath, "appLog.txt");
};
#define LOG_CLEAR TSimpleLog().Clear();
#define LOG(VALUE) TSimpleLog().Log(VALUE);

#define AND_OR_ENUM(TYPE)\
    constexpr bool operator & (TYPE lhs, TYPE rhs) { return static_cast<int>(lhs) & static_cast<int>(rhs); }\
    constexpr TYPE operator & (TYPE lhs, int rhs) { return static_cast<TYPE>(static_cast<int>(lhs) & rhs); }\
    constexpr TYPE operator | (TYPE lhs, TYPE rhs) { return static_cast<TYPE>(static_cast<int>(lhs) | static_cast<int>(rhs)); }\
    constexpr int operator ~(TYPE lhs) { return ~static_cast<int>(lhs); }\
    constexpr int operator *(TYPE lhs) { return static_cast<int>(lhs); }

#define FOR_ENUM(BEGIN, LAST, ...) \
    for(int i = static_cast<int>(BEGIN); i < static_cast<int>(LAST); i++)\
    {\
        auto e = static_cast<decltype(BEGIN)>(i);\
        __VA_ARGS__;\
    }
#endif //TESTAPP_TYPES_H
