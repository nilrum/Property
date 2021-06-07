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
    historyStack++;
    items[posItem]->Back();
    historyStack--;
    posItem--;
    OnChanged();
}

void THistory::Next()
{
    if(IsEnabledNext() == false) return;
    historyStack++;
    items[posItem + 1]->Next();
    historyStack--;
    posItem++;
    OnChanged();
}

void THistory::AddItem(const TPtrHistoryItem &value)
{
    if(historyStack) return;

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

TString THistory::Trans(const TString &value)
{
    if(trans) return trans(value);
    return value;
}

void THistory::SetTrans(const TFunTrans &value)
{
    trans = value;
}

bool THistory::IsSavePoint() const
{
    return posItem == savePoint;
}

void THistory::SetIsSavePoint()
{
    savePoint = posItem;
}
