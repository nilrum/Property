//
// Created by user on 08.04.2020.
//

#ifndef NEO_ENUMINFO_H
#define NEO_ENUMINFO_H
/*
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER)
#define IS_MAGIC_USED
#include "magic_enum.hpp"

#define ENUM_NAMES(NAME, ...)
#define ENUM(NAME, ...) enum NAME { __VA_ARGS__};
#define ENUM_CLASS(NAME, ...) enum class NAME { __VA_ARGS__};

#else*/

#include "Types.h"
class TEnum{
public:
    struct TEnumInfo{
        TVecString names;
        TVecDouble values;
        bool isValid;
        int Index(double value) const
        {
            for(size_t i = 0; i < values.size(); i++)
                if(values[i] == value) return i;
            return -1;
        }
        TString NameFromValue(double value) const
        {
            int index = Index(value);
            if(index != -1) return names[index];
            else return TString();
        }
    };
    using TEnumMap = std::map<TString, TEnumInfo>;
    using TEnumMapType = std::map<const std::type_info*, TString>;
    static bool Register(const TString& type, const std::type_info& inf, const TString& names, const TVecDouble& values = TVecDouble())
    {
        return Register(type, inf, SplitTrim(names, ','), values);
    }

    static bool Register(const TString& type, const std::type_info& inf, const TVecString& names, const TVecDouble& values = TVecDouble())
    {
        TEnumMap::const_iterator it = Enums().find(type);
        if(it != Enums().end()) return true;
        Enums()[type] = {names, values, true};
        EnumsIds()[&inf] = type;
        return true;
    }

    static const TVecString& EnumItems(const std::type_info& inf)
    {
        TEnumMapType::const_iterator it = EnumsIds().find(&inf);
        if(it != EnumsIds().end()) return EnumNames(it->second);
        return Single<TVecString>();
    }

    static const TVecString& EnumNames(const TString& type)
    {
        TEnumMap::const_iterator it = Enums().find(type);
        if(it != Enums().end()) return it->second.names;
        return Single<TVecString>();
    }

    static const TVecDouble& EnumValues(const TString& type)
    {
        TEnumMap::const_iterator it = Enums().find(type);
        if(it != Enums().end()) return it->second.values;
        return Single<TVecDouble>();
    }

    static const TEnumInfo& EnumInfo(const TString& type)
    {
        TEnumMap::const_iterator it = Enums().find(type);
        if(it != Enums().end()) return it->second;
        return Single<TEnumInfo>();
    }

    static const TEnumInfo& EnumInfo(const std::type_info& inf)
    {
        TEnumMapType::const_iterator it = EnumsIds().find(&inf);
        if(it != EnumsIds().end()) return EnumInfo(it->second);
        return Single<TEnumInfo>({TVecString(), TVecDouble(), false});
    }

    static int EnumIndexFromValue(const TString& type, double value)
    {
        return EnumInfo(type).Index(value);
    }
private:
    STATIC(TEnumMap, Enums);
    STATIC(TEnumMapType, EnumsIds);
};

template <typename T>
    double EnumVal(int index)
    {
        const TEnum::TEnumInfo& inf = TEnum::EnumInfo(typeid(T));
        if(index < inf.values.size()) return inf.values[index];
        return 0;
    }

#define ENUM_NAMES(NAME, ...)\
	namespace{ const bool b_##NAME = TEnum::Register(#NAME, typeid(NAME), #__VA_ARGS__); };


#define ENUM(NAME, ...)\
	enum NAME { __VA_ARGS__}; \
	ENUM_NAMES(NAME, __VA_ARGS__)

#define ENUM_CLASS(NAME, ...)\
	enum class NAME { __VA_ARGS__}; \
	ENUM_NAMES(NAME, __VA_ARGS__)

#define ENUM_VALS(NAME, NAMES, VALS)\
    enum class NAME { NAMES }; \
	namespace{ const bool b_##NAME = TEnum::Register(#NAME, typeid(NAME), TEXT_EN(NAMES), { VALS }); };

#define VAR(...) __VA_ARGS__
#define TEXT_EN(...) #__VA_ARGS__

//#endif
#endif //NEO_ENUMINFO_H
