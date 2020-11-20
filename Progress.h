//
// Created by user on 13.10.2020.
//

#ifndef NEO_PROGRESS_H
#define NEO_PROGRESS_H

#include "Types.h"
#include <mutex>
#include "sigslot/signal.hpp"

using TLock = std::lock_guard<std::mutex>;

class TProgress{
public:
    virtual ~TProgress(){};
    enum TTypeProgress{tpAbsolut = 0, tpStep};

    //The functions do not call GUI functions
    bool IsSend() const;
    virtual TProgress& SetIsSend(bool value);

    double Border() const;
    TProgress& SetBorder(double value);
    void SetBorderOfMax(double coef);

    TTypeProgress TypeProgress() const;
    TProgress& SetTypeProgress(TTypeProgress value);

    const TString& Text() const;
    TProgress& SetText(const TString& value);

    double Max() const;
    TProgress& SetMax(double value);

    TProgress& SetMaxAndBorderCoef(double value, double coef);

    void Progress(double value);//отправить значение прогресса
    void Finish();//отправить максимум и закрыть диалог

    void SetError(TResult value);

    sigslot::signal<TResult> OnFinish;
protected:
    TTypeProgress typeProg = tpAbsolut;
    std::mutex mut;

    double cur = 0.;        //текущее положение прогресса
    double maxProg = 10.;   //максимальное значение прогресса
    double borderProg = 0.; //пороговое значение для обновления отображения прогресса
    double curBorder = 0.;  //текуший порог обновления
    TString text;
    TResult error;
    bool isSend = false;    //используется режим пересылки прогресса TODO
    bool isChanged = true;  //изменились ли параметры прогресса

    virtual void ViewShow(){};
};

using TPtrProgress = std::shared_ptr<TProgress>;

#include <functional>
#include <thread>

class TPoolFunction{
public:
    using TCallFunction = std::function<TResult()>;

    static TPoolFunction* Create(TPtrProgress progress = TPtrProgress());
    void Start();
    void Reset();
    void Add(TCallFunction value);
    TPtrProgress& Progress() { return progress; };

    sigslot::signal<TResult> OnFinish; //сигнал о том что обработка завершена
    sigslot::signal<> OnStep; //сигнал о том что один из элементов списка отработал
private:
    TPoolFunction(){};
    TPtrProgress progress;
    std::unique_ptr<TPoolFunction> ptr;
    std::mutex mut;
    std::vector<TCallFunction> functions;
    std::atomic<int> results;
    TResult error;
};
#endif //NEO_PROGRESS_H
