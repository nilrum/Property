//
// Created by user on 12.11.2020.
//

#include "History.h"

INIT_PROPERTYS(THistory)

THistory::THistory()
{

}

bool THistory::IsEnabledBack() const
{
    return items.size() && posItem > -1;
}

bool THistory::IsEnabledNext() const
{
    return posItem < int(items.size() - 1);
}

void THistory::Back()
{
    if(IsEnabledBack() == false) return;
    items[posItem]->Back();
    posItem--;
    OnChanged();
}

void THistory::Next()
{
    if(IsEnabledNext() == false) return;
    items[posItem + 1]->Next();
    posItem++;
    OnChanged();
}

void THistory::AddItem(const TPtrHistoryItem &value)
{
    if(posItem != int(items.size() - 1))//если мы не в конце истории
    {                                               //то надо удалить сначало что переписываем
        items.erase(items.begin() + (posItem + 1), items.end());
    }
    if(items.empty() || items.back()->MergeItem(value.get()) == false)
        items.push_back(value);
    posItem = items.size() - 1;
    OnChanged();
}

void THistory::Clear()
{
    posItem = -1;
    items.clear();
    OnChanged();
}
