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
    virtual bool MergeItem(THistoryItem* value) { return false; }//можно ли объединить два элемента
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

#define HISTORY(TYPE, ...)\
    { if(THistory::IsUsed()) THistory::Single()->AddItem(std::make_shared<TYPE>(__VA_ARGS__)); }

#define HISTORY_ITEM(VALUE) \
    { if(THistory::IsUsed()) THistory::Single()->AddItem(VALUE); }

#define HISTORY_BEFORE(TYPE, ...) \
    std::shared_ptr<TYPE> item = std::make_shared<TYPE>(__VA_ARGS__); \

#define HISTORY_AFTER(FUNC) FUNC; HISTORY_ITEM(item)

#define HISTORY_CLEAR() { if(THistory::IsUsed()) THistory::Single()->Clear(); }

#endif //NEO_HISTORY_H
