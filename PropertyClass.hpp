//
// Created by user on 27.08.2019.
//

#include "PropertyClass.h"

template<class T>
T VariableToPropertyClassImpl(const TVariable &value)
{
    TPtrPropertyClass baseRez = value.ToType<TPtrPropertyClass>();
    return std::dynamic_pointer_cast<typename T::element_type>(baseRez);
}

template<typename T>
struct TIsClass{
    using TCheck = typename std::decay<T>::type;
    static const bool value = !(std::is_arithmetic<TCheck>::value || std::is_enum<TCheck>::value
            || std::is_same<TCheck, TString>::value || std::is_same<TCheck, char*>::value
            || std::is_same<TCheck, TEnum>::value || std::is_same<TCheck, TVariable>::value);
};

template<typename T, typename R>
std::enable_if_t<std::is_base_of<TPropertyClass, R>::value, TGetFun> GetFun(std::shared_ptr<R> (T::*method)() const)
{
    return [method](const TPropertyClass *ptr) {
        const T *obj = dynamic_cast<const T *>(ptr);
        if (obj == nullptr) return TVariable();
        return PropertyClassToVariable((obj->*method)());
    };
}

template<typename T, typename R, typename TInd>
std::enable_if_t<std::is_base_of<TPropertyClass, R>::value, TGetFun> GetFun(std::shared_ptr<R> (T::*method)() const, TInd index)
{
    return [method, index](const TPropertyClass *ptr) {
        const T *obj = dynamic_cast<const T *>(ptr);
        if (obj == nullptr) return TVariable();
        return PropertyClassToVariable((obj->*method)(index));
    };
}

template<typename T, typename R>
TGetFun GetFun(R (T::*method)() const)
{
    return [method](const TPropertyClass *ptr) {
        const T *obj = dynamic_cast<const T *>(ptr);
        if (obj) return TVariable((obj->*method)());
        else return TVariable();
    };
}
//вариант для вызова с индексом по умолчанию
template<typename T, typename R, typename TInd>
TGetFun GetFun(R (T::*method)(TInd) const, TInd index)
{
    return [method, index](const TPropertyClass *ptr) {
        const T *obj = dynamic_cast<const T *>(ptr);
        if (obj) return TVariable((obj->*method)(index));
        else return TVariable();
    };
}

template<typename T, typename R, typename P>
std::enable_if_t<std::is_base_of<TPropertyClass, P>::value, TSetFun> SetFun(R (T::*method)(const std::shared_ptr<P> &))
{
    return [method](TPropertyClass *ptr, const TVariable &value) {
        T *obj = dynamic_cast<T *>(ptr);
        if (obj) (obj->*method)(VariableToPropertyClassImpl<std::shared_ptr<P>>(value));
    };
}
//вариант для вызова с индексом по умолчанию
template<typename T, typename R, typename P, typename TInd>
std::enable_if_t<std::is_base_of<TPropertyClass, P>::value, TSetFun> SetFun(R (T::*method)(TInd, const std::shared_ptr<P> &), TInd index)
{
    return [method, index](TPropertyClass *ptr, const TVariable &value) {
        T *obj = dynamic_cast<T *>(ptr);
        if (obj) (obj->*method)(index, VariableToPropertyClassImpl<std::shared_ptr<P>>(value));
    };
}

template<typename T, typename R, typename P>
TSetFun SetFun(R (T::*method)(P))
{
    return [method](TPropertyClass *ptr, const TVariable &value) {
        T *obj = dynamic_cast<T *>(ptr);
        using TBase = std::remove_reference_t<std::remove_const_t<P>>;
        if constexpr( std::is_enum_v<TBase> == false )
        {
            if (obj) (obj->*method)(value);
        }
        else
        {
            if (obj) (obj->*method)(value.ToEnum<TBase>());
        }

    };
}
//вариант для вызова с индексом по умолчанию
template<typename T, typename R, typename P, typename TInd>
TSetFun SetFun(R (T::*method)(TInd, P), TInd index)
{
    return [method, index](TPropertyClass *ptr, const TVariable &value) {
        T *obj = dynamic_cast<T *>(ptr);
        if (obj) (obj->*method)(index, value);
    };
}

template<typename T, typename R, typename TInd>
TGetIndFun GetIndFun(R (T::*method)(TInd) const)
{
    return [method](const TPropertyClass *ptr, TInd index) {
        const T *obj = dynamic_cast<const T *>(ptr);
        if (obj == nullptr) return TVariable();
        return PropertyClassToVariable((obj->*method)(index));
    };
}

#define PROPERTIES_BASE_CHK(TYPE, CREATE, CHECK)\
    public:\
        STATIC_ARG(TPropertyManager, ManagerStatic, #TYPE, CREATE) \
        static TString TypeStatic() { return ManagerStatic().Type(); } \
        virtual TPropertyManager& Manager() CHECK { return TYPE::ManagerStatic(); }\
        virtual const TPropertyManager& Manager() const CHECK { return TYPE::ManagerStatic(); }\
        using TYPENAME = TYPE;                  \
        template<typename... TArgs>            \
        static std::shared_ptr<TYPE> Create(TArgs&&... args) \
        {                                      \
            return std::shared_ptr<TYPE>(new TYPE(std::forward<TArgs>(args)...)); \
        }
#define DEF_CREATE(TYPE) [](){ return std::make_shared<TYPE>(); }
#define NO_CREATE() TPropertyManager::TFunCreate()
#define SHARED_CREATE(TYPE) [](){ return TYPE::CreateShared(); }

#define PROPERTIES_BASE(TYPE) PROPERTIES_BASE_CHK(TYPE, DEF_CREATE(TYPE),)

#define PROPERTIES_CREATE(TYPE, BASE, CREATE, ...)\
    PROPERTIES_BASE_CHK(TYPE, CREATE, override)\
    static bool InitProperties() noexcept{\
        if(TYPE::ManagerStatic().IsInit()) return true;\
        BASE::InitProperties();\
        TYPE::ManagerStatic().AppendProperties(BASE::ManagerStatic());\
        __VA_ARGS__\
        TYPE::ManagerStatic().SetIsInit();\
        return true;\
    }

#define PROPERTIES(TYPE, BASE, ...) PROPERTIES_CREATE(TYPE, BASE, DEF_CREATE(TYPE), __VA_ARGS__)
#define PROPERTIES_SHARED(TYPE, BASE, ...) \
    template<typename... TArgs>            \
    static std::shared_ptr<TYPE> CreateShared(TArgs&&... args) \
    {                                      \
        auto res = std::shared_ptr<TYPE>(new TYPE(std::forward<TArgs>(args)...)); \
        res->thisWeak = res;               \
        return res;                        \
    } \
    private:                       \
        std::weak_ptr<TYPE> thisWeak;\
    PROPERTIES_CREATE(TYPE, BASE, SHARED_CREATE(TYPE), __VA_ARGS__)


#define INIT_PROPERTYSN(NAME, TYPE) namespace { const bool init##NAME = TYPE::InitProperties(); }
#define INIT_PROPERTYS(TYPE) INIT_PROPERTYSN(TYPE, TYPE)

#define APROPERTY_NAME(TYPE, NAME) ManagerStatic().AddProperty(#TYPE, NAME, TIsClass<TYPE>::value)
#define APROPERTY(TYPE, NAME) APROPERTY_NAME(TYPE, #NAME)
#define PROPERTY_READ(TYPE, NAME, GET) APROPERTY(TYPE, NAME).Get(GetFun(&TYPENAME::GET))
#define PROPERTY(TYPE, NAME, GET, SET) PROPERTY_READ(TYPE, NAME, GET).Set(SetFun(&TYPENAME::SET))
#define PROPERTY_ARRAY_READ(TYPE, NAME, COUNT, GET) \
            APROPERTY(TYPE, NAME).GetArray(GetFun(&TYPENAME::COUNT), GetIndFun(&TYPENAME::GET))
#define PROPERTY_ARRAY_ADD(TYPE, NAME, COUNT, GET, ADD) \
            PROPERTY_ARRAY_READ(TYPE, NAME, COUNT, GET).AddArray(SetFun(&TYPENAME::ADD))
#define PROPERTY_ARRAY(TYPE, NAME, COUNT, GET, ADD, DEL) \
            PROPERTY_ARRAY_ADD(TYPE, NAME, COUNT, GET, ADD).DelArray(SetFun(&TYPENAME::DEL))

#define PROPERTY_CALL_IMPL(GET, SET, INDEX)\
     Get(GetFun(&TYPENAME::GET, size_t(INDEX))).Set(SetFun(&TYPENAME::SET, size_t(INDEX)))

#define PROPERTY_CALL(TYPE, NAME, GET, SET, INDEX)\
     APROPERTY(TYPE, NAME).PROPERTY_CALL_IMPL(GET, SET, INDEX)

#define PROPERTY_FUN_IMPL(TYPE, NAME, GET, SET, OTHER)\
    public:\
        TYPE GET() const { return NAME; };\
        TYPENAME& SET(const TYPE& value) { bool chg = NAME != value; NAME = value; if(chg) { OTHER; } return *this; }

#define PROPERTY_FUN(TYPE, NAME, GET, SET)\
    public:\
        TYPE GET() const { return NAME; };\
        TYPENAME& SET(const TYPE& value) { NAME = value; return *this; }

#define PROPERTY_FUN_CHG(TYPE, NAME, GET, SET)\
    public:\
        TYPE GET() const { return NAME; };\
        TYPENAME& SET(const TYPE& value) { bool chg = NAME != value; NAME = value; if(chg) OnChanged(); return *this; }

#define PROPERTY_ARRAY_READ_FUN(TYPE, NAME, COUNT, GET)\
    public:\
        size_t COUNT() const { return NAME.size(); }\
        const TYPE& GET(size_t index) const { return NAME[index]; }

#define PROPERTY_ARRAY_ADD_FUN_IMPL(TYPE, NAME, COUNT, GET, ADD, OTHER)\
        PROPERTY_ARRAY_READ_FUN(TYPE, NAME, COUNT, GET)\
        TYPE& ADD(const TYPE& value){ NAME.push_back(value); OTHER; return NAME.back(); }


#define PROPERTY_ARRAY_FUN_IMPL(TYPE, NAME, COUNT, GET, ADD, DEL, OTHER)\
    PROPERTY_ARRAY_ADD_FUN_IMPL(TYPE, NAME, COUNT, GET, ADD, OTHER)\
        void DEL(const TYPE& value){ \
            for(auto it = NAME.begin(); it != NAME.end(); it++) if(*it == value){ NAME.erase(it); break; }; OTHER; }

#define PROPERTY_ARRAY_FUN(TYPE, NAME, COUNT, GET, ADD, DEL)\
    PROPERTY_ARRAY_FUN_IMPL(TYPE, NAME, COUNT, GET, ADD, DEL,)

#define PROPERTY_ARRAY_FUN_CHG(TYPE, NAME, COUNT, GET, ADD, DEL) PROPERTY_ARRAY_FUN_IMPL(TYPE, NAME, COUNT, GET, ADD, DEL, OnChanged())