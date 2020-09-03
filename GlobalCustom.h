//
// Created by user on 24.04.2020.
//

#ifndef NEO_GLOBALCUSTOM_H
#define NEO_GLOBALCUSTOM_H

#include "PropertyClass.h"
class TGlobalCustom;
using TPtrGlobalCustom = std::shared_ptr<TGlobalCustom>;

class TGlobalCustom : public TPropertyClass{
public:
    TGlobalCustom();

    TPtrPropertyClass FindCustom(const TString& path);
    STATIC_ARG(TPtrGlobalCustom, Single, std::make_shared<TGlobalCustom>());
    static bool Register(const TPtrPropertyClass& value);

    PROPERTIES(TGlobalCustom, TPropertyClass,
        PROPERTY_ARRAY(TPropertyClass, customs, CountCustoms, Custom, AddCustom, DelCustom).SetCheckName(true);
        );
    PROPERTY_ARRAY_FUN(TPtrPropertyClass, customs, CountCustoms, Custom, AddCustom, DelCustom);
private:
    std::vector<TPtrPropertyClass> customs;
};

#define REGISTER_CUSTOM(VALUE)\
    namespace{\
        const bool cust = TGlobalCustom::Register(VALUE);\
    }

#endif //NEO_GLOBALCUSTOM_H
