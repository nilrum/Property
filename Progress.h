//
// Created by user on 13.10.2020.
//

#ifndef NEO_PROGRESS_H
#define NEO_PROGRESS_H

#include "Types.h"

class TProgress{
public:
    enum TTypeProgress{tpAbsolut = 0, tpStep};

    //The functions do not call GUI functions
    bool IsSend() const;
    TProgress& SetIsSend(bool value);

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

protected:
    TTypeProgress typeProg = tpAbsolut;
    double cur = 0.;        //текущее положение прогресса
    double maxProg = 10.;   //максимальное значение прогресса
    double borderProg = 0.; //пороговое значение для обновления отображения прогресса
    double curBorder = 0.;  //текуший порог обновления
    TString text;
    bool isSend = false;    //используется режим пересылки прогресса TODO
    bool isChanged = true;  //изменились ли параметры прогресса

    virtual void ViewShow(){};
    virtual void Cancel(){};
};

using TPtrProgress = std::shared_ptr<TProgress>;

#endif //NEO_PROGRESS_H
