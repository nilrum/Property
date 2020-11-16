//
// Created by user on 12.11.2020.
//

#ifndef NEO_HISTORY_H
#define NEO_HISTORY_H

#include "PropertyClass.h"

class THistoryItem : public TPropertyClass{
public:
    THistoryItem(const TString& text = TString()){ name = text; }
    virtual void Back(){};
    virtual void Next(){};
    PROPERTIES(THistoryItem, TPropertyClass,)
};

using TPtrHistoryItem = std::shared_ptr<THistoryItem>;
class THistory;
using TPtrHistory = std::shared_ptr<THistory>;

class THistory : public TPropertyClass{
public:
    THistory();

    void Clear();

    bool IsEnabledBack() const;
    bool IsEnabledNext() const;

    void Back();
    void Next();

    PROPERTIES(THistory, TPropertyClass,
           PROPERTY_ARRAY_READ(THistoryItem, items, CountItems, Item);
           )
    inline int PosItem() const { return posItem; }

    PROPERTY_ARRAY_READ_FUN(TPtrHistoryItem, items, CountItems, Item);
    void AddItem(const TPtrHistoryItem& value);
    STATIC_ARG(bool, IsUsed, true)
    STATIC_ARG(TPtrHistory, Single, std::make_shared<THistory>())
protected:
    int posItem = -1;
    std::vector<TPtrHistoryItem> items;
};



#endif //NEO_HISTORY_H
