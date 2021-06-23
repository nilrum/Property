//
// Created by user on 08.04.2020.
//

#ifndef NEO_ENUMINFO_H
#define NEO_ENUMINFO_H

#include "Types.h"

class TEnum;

class TEnumInfo {
private:
    TString typeEnum;
    const std::type_info *info;
    TVecString names;       //название элементов множества
    TVecDouble values;      //возможное значение элемента
    bool isValid;           //валидная ли информация
    int convCat = 0;
    int convFrom = 0;
    using TVecEnum = std::vector<TEnumInfo>;
    using TMapInfo = std::map<const std::type_info *, int>;
    using TMapType = std::map<TString, int>;

    STATIC(TVecEnum, Enums);
    STATIC(TMapInfo, EnumsInfos);
    STATIC(TMapType, EnumsTypes);
public:
    TEnumInfo() : info(nullptr), isValid(false){};

    TEnumInfo(const TString& type, const std::type_info *inf, const TVecString &ns, const TVecDouble &vs = TVecDouble()) :
           typeEnum(type), info(inf), names(ns), values(vs), isValid(true){};

    const TString& TypeEnum() const { return typeEnum; }
    const TVecString &Names() const { return names; }
    const TVecDouble &Values() const { return values; }
    const std::type_info &Info() const { return *info; }

    inline size_t CountNames() const { return names.size(); }

    inline TString Name(int index) const
    {
        if (index >= 0 && index < int(names.size())) return names[index];
        return TString();
    }

    inline double Value(int index) const
    {
        if (index >= 0 && index < int(values.size())) return values[index];
        return 0.;
    }

    int IndexFromName(const TString &name) const
    {
        for (size_t i = 0; i < names.size(); i++)
            if (names[i] == name) return i;
        return -1;
    }

    int IndexFromValue(double value) const  //возвращает номер по порядкук по значению
    {
        for (size_t i = 0; i < values.size(); i++)
            if (values[i] == value) return i;
        return -1;
    }

    inline TString NameFromValue(double value) const
    {
        int index = IndexFromValue(value);
        return Name(index);
    }

    inline double ValueFromName(TString name) const
    {
        int index = IndexFromName(name);
        return Value(index);
    }

    inline bool IsValid() const { return isValid; }

    inline void ConvertInfo(int cat, int from)
    {
        convCat = cat;
        convFrom = from;
    }

    inline int ConvertCategory() const { return convCat; }
    inline int ConvertFrom() const { return convFrom; }

    TEnum FromIndex(int index) const;
    TEnum FromValue(double value) const;

    static bool Register(const TString &type, const std::type_info &inf, const TVecString &names,
                         const TVecDouble &values = TVecDouble(), int cat = 0, int from = 0);

    static bool Register(const TString &type, const std::type_info &inf, const TString &names,
                         const TVecDouble &values = TVecDouble(), int cat = 0, int from = 0);

    static const TEnumInfo &EnumInfo(const std::type_info &inf);

    static const TEnumInfo &EnumInfo(const TString &type);
};

template <typename T>
const TString& TypeEnum()
{
    return TEnumInfo::EnumInfo(typeid(T)).TypeEnum();
}
template <typename T>
double EnumValue(int index)
{
    return TEnumInfo::EnumInfo(typeid(T)).Value(index);
}

template <typename T>
TString EnumName(int index)
{
    return TEnumInfo::EnumInfo(typeid(T)).Name(index);
}

template <typename T>
TString EnumName(const T& index)
{
    return TEnumInfo::EnumInfo(typeid(T)).Name(static_cast<int>(index));
}

class TEnum{
private:
    const std::type_info* info = nullptr;
    int index = 0;
    friend TEnumInfo;
    TEnum(const std::type_info* inf, int ind):info(inf), index(ind){}
public:
    TEnum(int i = 0):index(i) {}
    template <typename T, typename = std::enable_if_t<std::is_enum_v<T>>>
        TEnum(const T& value)
    {
        index = static_cast<int>(value);
        info = &typeid(T);
    }
    inline TEnum& SetIndex(int value) { index = value; return *this; }

    operator int() const { return index; }

    inline bool IsValid() const { return info != nullptr; }
    inline int64_t Index() const { return index; }
    inline const std::type_info& Info() const { return *info; }
    inline TString Name() const { return TEnumInfo::EnumInfo(*info).Name(index); }
    inline const TVecString& Names() const { return TEnumInfo::EnumInfo(*info).Names(); }
    inline TString TypeEnum() const { return TEnumInfo::EnumInfo(*info).TypeEnum(); }
};


#define ENUM_NAMES(NAME, ...)\
	namespace{ const bool b_##NAME = TEnumInfo::Register(#NAME, typeid(NAME), #__VA_ARGS__); };


#define ENUM(NAME, ...)\
	enum NAME { __VA_ARGS__ }; \
	ENUM_NAMES(NAME, __VA_ARGS__)

#define ENUM_CLASS(NAME, ...)\
	enum class NAME { __VA_ARGS__}; \
	ENUM_NAMES(NAME, __VA_ARGS__)


#define ENUM_VALS_CONV(NAME, CAT, FROM, NAMES, VALS)\
    enum NAME { NAMES }; \
	namespace{ const bool b_##NAME = TEnumInfo::Register(#NAME, typeid(NAME), TEXT_EN(NAMES), { VALS }, CAT, FROM); };

#define ENUM_VALS(NAME, NAMES, VALS) \
    enum NAME { NAMES }; \
	namespace{ const bool b_##NAME = TEnumInfo::Register(#NAME, typeid(NAME), TEXT_EN(NAMES), { VALS }); };

#define VAR(...) __VA_ARGS__
#define TEXT_EN(...) #__VA_ARGS__

#endif //NEO_ENUMINFO_H
