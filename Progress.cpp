//
// Created by user on 13.10.2020.
//

#include "Progress.h"


void TProgress::Progress(double value)
{
    switch (typeProg)
    {
        case tpAbsolut:
            cur = value;
            break;

        case tpStep:
            cur = cur + value;
            break;
    }
    if(value != 0. && borderProg != 0.)
    {
        if (TDoubleCheck::Less(cur, curBorder)) return;         //если бордюр не преодолели то окно прогресса не вызываем
        curBorder = curBorder + borderProg; //если бордюр преодолен, то отобразим окно
    }

    ViewShow();
    if(cur >= maxProg) Cancel();
}

void TProgress::Finish()
{
    Progress(maxProg + 1);
}

bool TProgress::IsSend() const
{
    return isSend;
}

TProgress &TProgress::SetIsSend(bool value)
{
    isChanged = isSend != value;
    isSend = value;
    return *this;
}

double TProgress::Border() const
{
    return borderProg;
}

TProgress &TProgress::SetBorder(double value)
{
    isChanged = borderProg != value;
    borderProg = value;
    curBorder = borderProg;
    return *this;
}

TProgress::TTypeProgress TProgress::TypeProgress() const
{
    return typeProg;
}

TProgress &TProgress::SetTypeProgress(TProgress::TTypeProgress value)
{
    isChanged = typeProg != value;
    typeProg = value;
    return *this;
}

const TString &TProgress::Text() const
{
    return text;
}

TProgress &TProgress::SetText(const TString &value)
{
    isChanged = text != value;
    text = value;
    return *this;
}

double TProgress::Max() const
{
    return maxProg;
}

TProgress &TProgress::SetMax(double value)
{
    isChanged = maxProg != value;
    maxProg = value;
    return *this;
}

void TProgress::SetBorderOfMax(double coef)
{
    SetBorder(maxProg * coef);
}

TProgress &TProgress::SetMaxAndBorderCoef(double value, double coef)
{
    SetMax(value);
    SetBorderOfMax(coef);
    return *this;
}
