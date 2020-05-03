//
// Created by user on 19.04.2020.
//

#include "EnumInfo.h"

bool TEnumInfo::Register(const TString& type, const std::type_info& inf, const TVecString& names, const TVecDouble& values, int cat, int from)
{
    TMapInfo::const_iterator it = EnumsInfos().find(&inf);
    if(it != EnumsInfos().end()) return false;

    Enums().emplace_back(type, &inf, names, values);
    Enums().back().ConvertInfo(cat, from);
    EnumsInfos()[&inf] = Enums().size() - 1;
    EnumsTypes()[type] = Enums().size() - 1;
    return true;
}

bool TEnumInfo::Register(const TString& type, const std::type_info& inf, const TString& names, const TVecDouble& values, int cat, int from)
{
    return TEnumInfo::Register(type, inf, SplitTrim(names, ','), values, cat, from);
}

const TEnumInfo& TEnumInfo::EnumInfo(const std::type_info& inf)
{
    TMapInfo::const_iterator it = EnumsInfos().find(&inf);
    if(it != EnumsInfos().end()) return Enums()[it->second];
    return Single<TEnumInfo>();
}

const TEnumInfo& TEnumInfo::EnumInfo(const TString& type)
{
    TMapType::const_iterator it = EnumsTypes().find(type);
    if(it != EnumsTypes().end()) return Enums()[it->second];
    return Single<TEnumInfo>();
}

TEnum TEnumInfo::FromIndex(int index) const
{
    return TEnum(info, index);
}

TEnum TEnumInfo::FromValue(double value) const
{
    return TEnum(info, IndexFromValue(value));
}
