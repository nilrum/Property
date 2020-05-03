//
// Created by user on 24.04.2020.
//

#include "GlobalCustom.h"

INIT_PROPERTYS(TGlobalCustom);

TGlobalCustom::TGlobalCustom()
{
    name = "GlobalCustom";
}

bool TGlobalCustom::Register(const TPtrPropertyClass &value)
{
    Single()->AddCustom(value);
    return true;
}
