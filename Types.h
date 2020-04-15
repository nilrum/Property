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

using TString = std::string;
using TVecString = std::vector<TString>;
using TVecDouble = std::vector<double>;
using TVecUInt = std::vector<uint32_t>;

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

class /* [[nodiscard]]*/ TRezult{
public:
    TRezult():info(&typeid(int)){};
    TRezult(int value):code(value), info(&typeid(int)){}
    template <typename T> TRezult(const T& value):code(static_cast<int>(value)), info(&typeid(T)){}
    TRezult(const TRezult& oth):code(oth.code), info(oth.info){};
    void operator =(const TRezult& oth){ code = oth.code; info = oth.info; }

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

    static TString TextError(const TRezult& value)
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
        const bool r##CODE = TRezult::Register(typeid(TYPE), static_cast<int>(TYPE::CODE), TEXT);\
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

//TODO сделать общий вариант работы

template<typename T>
struct TListFun{
    using TFuns = std::map<int, T>;
    TFuns funs;
    int last = 0;
    inline explicit operator bool() const
    {
        return funs.size();
    }
    inline void operator ()()
    {
        Call();
    }
    int Add(const T& value)
    {
        funs[last] = value;
        return last++;
    }
    void Del(int id)
    {
        typename TFuns::const_iterator it = funs.find(id);
        if(it != funs.end())
            funs.erase(it);
    }
    inline void Call()
    {
        TFuns temp = funs;
        for(typename TFuns::const_iterator it = temp.begin(); it != temp.end(); it++)
            it->second();
    }
};
#endif //TESTAPP_TYPES_H
