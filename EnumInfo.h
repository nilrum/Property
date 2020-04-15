//
// Created by user on 08.04.2020.
//

#ifndef NEO_ENUMINFO_H
#define NEO_ENUMINFO_H

#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER)
#define IS_MAGIC_USED
#include "magic_enum.hpp"

#define ENUM_NAMES(NAME, ...)
#define ENUM(NAME, ...) enum NAME { __VA_ARGS__};
#define ENUM_CLASS(NAME, ...) enum class NAME { __VA_ARGS__};

#else

#include "Types.h"
class TEnum{
public:
    using TEnumMap = std::map<TString, TVecString>;
    using TEnumMapType = std::map<const std::type_info*, TString>;
    static bool Register(const TString& type, const std::type_info& inf, const TString& values)
    {
        return Register(type, inf, SplitTrim(values, ','));
    }

    static bool Register(const TString& type, const std::type_info& inf, const TVecString& values)
    {
        TEnumMap::const_iterator it = Enums().find(type);
        if(it != Enums().end()) return true;
        Enums()[type] = values;
        EnumsIds()[&inf] = type;
        return true;
    }

    static const TVecString& EnumItems(const std::type_info& inf)
    {
        TEnumMapType::const_iterator it = EnumsIds().find(&inf);
        if(it != EnumsIds().end()) return EnumItems(it->second);
        return Single<TVecString>();
    }

    static const TVecString& EnumItems(const TString& type)
    {
        TEnumMap::const_iterator it = Enums().find(type);
        if(it != Enums().end()) return it->second;
        return Single<TVecString>();
    }
private:
    STATIC(TEnumMap, Enums);
    STATIC(TEnumMapType, EnumsIds);
};

#define ENUM_NAMES(NAME, ...)\
	namespace{ const bool b_##NAME = TEnum::Register(#NAME, typeid(NAME), #__VA_ARGS__); };

#define ENUM(NAME, ...)\
	enum NAME { __VA_ARGS__}; \
	ENUM_NAMES(NAME, __VA_ARGS__)

#define ENUM_CLASS(NAME, ...)\
	enum class NAME { __VA_ARGS__}; \
	ENUM_NAMES(NAME, __VA_ARGS__)

#endif
#endif //NEO_ENUMINFO_H
