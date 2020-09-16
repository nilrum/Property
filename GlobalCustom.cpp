//
// Created by user on 24.04.2020.
//

#include "GlobalCustom.h"

INIT_PROPERTYS(TGlobalCustom);
INIT_PROPERTYS(TTargetCustom);

TGlobalCustom::TGlobalCustom()
{
    name = "GlobalCustom";
}

bool TGlobalCustom::Register(const TPtrPropertyClass &value)
{
    Single()->AddCustom(value);
    return true;
}
//customs:PlotCustom/graphs:Contstruction/columns:1
TPtrPropertyClass TGlobalCustom::FindCustom(const TString &path)
{
    TPtrPropertyClass cur = shared_from_this();
    TVecString paths = SplitTrim(path, '/');
    int index = 1;//по умолчанию индекс для поиска 1, т.к. 0 = name
    int ind = 0;
    for(const auto& p : paths)
    {
        TVecString pn = SplitTrim(p, ':');
        index = (pn.size() > 1)? cur->IndexProperty(pn[0]) : 1;
        if(index >= 0 && index < int(cur->CountProperty()))//свойство для поиска есть ищим
        {
            int count = cur->CountInArray(index);
            if(count && TryStrToInt(pn.back(), ind))//если можно привести к числу то берем его как  индекс в массиве свойства
            {
                cur = VariableToPropClass(cur->ReadFromArray(index, ind));
                break;
            }
            else
                for(int i = 0; i < count; i++)
                {
                    TPtrPropertyClass ptr = VariableToPropClass(cur->ReadFromArray(index, i));
                    if(ptr->Name() == pn.back())
                    {
                        cur = ptr;
                        break;
                    }
                }
        }
    }
    if(cur != shared_from_this())
        return cur;
    return TPtrPropertyClass();
}
