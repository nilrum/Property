//
// Created by user on 24.09.2019.
//

#ifndef TESTAPP_TYPES_H
#define TESTAPP_TYPES_H

#include <string>
#include <vector>
#include <map>
#include <algorithm>

using TString = std::string;
using TVecString = std::vector<TString>;

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

class TRezult{
public:
    TRezult():info(typeid(int)){};
    TRezult(int value):code(value), info(typeid(int)){}
    template <typename T> TRezult(const T& value):code(static_cast<int>(value)), info(typeid(T)){}

    //inline operator int() const { return code; }
    //inline operator bool() const { return IsNoError(); }
    inline bool IsNoError() const { return code == 0; }
    inline bool IsError() const { return code != 0; }
    template<typename T>
        inline bool Is(const T& value) { return static_cast<T>(code) == value && info == typeid(T); }

    int Code() const { return code; }
    const std::type_info& Info() const { return info; };

    static bool Register(const std::type_info& info, int code, const TString& text)
    {
       Enums()[info.name()][code] = text;
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
    int code = 0;
    const std::type_info& info;

};

#define REGISTER_CODES(TYPE, CODE, TEXT)\
    namespace{\
        const bool r##CODE = TRezult::Register(typeid(TYPE), static_cast<int>(TYPE::CODE), TEXT);\
    }


template<typename TVec>
    bool RemoveVal(TVec& vec, typename TVec::value_type& val)
{
        typename TVec::iterator it = std::find(vec.begin(), vec.end(), val);
        if(it == vec.end()) return false;
        vec.erase(it);
        return true;
}
#endif //TESTAPP_TYPES_H
