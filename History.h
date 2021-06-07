//
// Created by user on 12.11.2020.
//

#ifndef NEO_HISTORY_H
#define NEO_HISTORY_H

#include "PropertyClass.h"

class THistoryItem : public TPropertyClass{
public:
    THistoryItem(const TString& text = TString()){ name = text; }
    virtual void Back(){ BackNext(true); };
    virtual void Next(){ BackNext(false); };
    virtual bool MergeItem(THistoryItem* value) { return false; }//можно ли объединить два элемента
    PROPERTIES(THistoryItem, TPropertyClass,)

protected:
    virtual void BackNext(bool isBack){};
};

class THistoryItemTime : public THistoryItem{
public:
    THistoryItemTime()
    {
        time = Clock::now();
    }
    PROPERTIES(THistoryItemTime, THistoryItem,)
protected:
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    TimePoint time;

    bool CheckTime(const THistoryItemTime& oth, const std::chrono::milliseconds& value)
    {
        return oth.time - time < value;
    }
};

using TPtrHistoryItem = std::shared_ptr<THistoryItem>;
class THistory;
using TPtrHistory = std::shared_ptr<THistory>;
using TFunTrans = std::function<TString(const TString&)>;
class THistory : public TPropertyClass{
public:
    THistory();

    void Clear();

    bool IsEnabledBack() const; //возвращает возможна ли отмена операции
    bool IsEnabledNext() const; //возвращает возможен ли повтор операции

    void Back();
    void Next();

    TString Trans(const TString& value);    //перевод для текста в истории(используется для уменьшения связности)
    void SetTrans(const TFunTrans& value);  //установить функцию перевода

    bool IsSavePoint() const;   //вовзращает является ли текущая точка в истории сохраненным значением
    void SetIsSavePoint();      //устанавливает текущую точку как сохраненную

    PROPERTIES(THistory, TPropertyClass,
           PROPERTY_ARRAY_READ(THistoryItem, items, CountItems, Item);
           )

    inline int PosItem() const { return posItem; }
    inline int PosSavePoint() const { return savePoint; }

    PROPERTY_ARRAY_READ_FUN(TPtrHistoryItem, items, CountItems, Item);
    void AddItem(const TPtrHistoryItem& value);
    STATIC_ARG(bool, IsUsed, true)
    STATIC_ARG(TPtrHistory, Single, std::make_shared<THistory>())
protected:
    int posItem = -1;
    int savePoint = -1;
    int historyStack = 0;//флаг для того чтобы во время операции Back или Next не было добавлений в очередь
    std::vector<TPtrHistoryItem> items;
    TFunTrans trans;
};

#define HISTORY THistory::Single()

#define HISTORY_IF(VALUE) if(THistory::IsUsed()) {VALUE}

#define HISTORY_CREATE(TYPE, ...)\
     HISTORY_IF(HISTORY->AddItem(std::make_shared<TYPE>(__VA_ARGS__));)

#define HISTORY_ADD(VALUE) \
     HISTORY_IF(HISTORY->AddItem(VALUE);)

#define HISTORY_BEFORE(TYPE, ...) \
    std::shared_ptr<TYPE> item = std::make_shared<TYPE>(__VA_ARGS__); \

#define HISTORY_AFTER(FUNC) FUNC; HISTORY_ADD(item)

#define HISTORY_VEC(BEGIN, END) \
     HISTORY_IF(for(auto it = BEGIN; it != END; it++) HISTORY->AddItem(*it);)

#define HISTORY_CLEAR() HISTORY_IF(HISTORY->Clear(); )

#define HISTORY_TRANS(VALUE) HISTORY->Trans(VALUE)
#define HISTORY_TRANSR(VALUE) STR(HISTORY_TRANS(VALUE))

#endif //NEO_HISTORY_H
