#include "completewiz.h"

#include "noisefillpage.h"
#include "peakfilterpage.h"
#include "qassurancepage.h"
#include "normpage.h"
#include "imputationpage.h"
#include "transfpage.h"
#include "scalpage.h"

#include <QPushButton>
#include <QDebug>

CompleteWiz::CompleteWiz(ProcessDF* df,QWidget* parent)
    :QWizard(parent)
{
    for(int i=0;i<7;i++)
        Operate.push_back(true);

    addPage(new NoiseFillPage(df));
    addPage(new PeakFilterPage);
    addPage(new QAssurancePage(df));
    addPage(new NormPage(df));
    addPage(new ImputationPage);
    addPage(new TransfPage(df));
    addPage(new ScalPage);

    QPushButton *but = new QPushButton("Skip");
    setButton(QWizard::CustomButton1,but);
    setOption(QWizard::HaveCustomButton1);

    DF=df;

    connect(this,SIGNAL(accepted()),this,SLOT(finalise()));
    connect(but,SIGNAL(clicked(bool)),this,SLOT(skip()));
    connect(button(QWizard::NextButton),SIGNAL(clicked(bool)),this,SLOT(NextBut()));
}

void CompleteWiz::skip()
{
    Operate[currentId()]=false;
    if(nextId()!=-1)
        next();
    else
        emit accept();
}

void CompleteWiz::NextBut()
{                                //here currentId is already on next page
    Operate[currentId()-1]=true; //so -1
}

void CompleteWiz::finalise()
{
    if(Operate[0])
        DF->NoiseFill(field("percent").toInt(),field("factor").toString(),Noisemsg);
    if(Operate[1])
        DF->PeakFilter(field("percFeat").toInt(),field("percSamp").toInt(),true,PeakFmsg);
    if(Operate[2])
        DF->QAssurance(field("QCLabQas").toString(),field("percRSD").toInt(),Qasmsg);
    if(Operate[3])
        DF->Normalisation(static_cast<ProcessDF::NormMode>(field("modeNorm").toInt()),field("QCLabNorm").toString(),Normmsg);
    if(Operate[4])
        DF->MVImpute(static_cast<ProcessDF::MVIMode>(field("modeMVI").toInt()),Impmsg);
    if(Operate[5])
        DF->Transform(static_cast<ProcessDF::TransformMode>(field("modeTransf").toInt()),field("QCLabTransf").toString(),Transfmsg);
    if(Operate[6])
        DF->Scale(static_cast<ProcessDF::ScalingMode>(field("modeScal").toInt()),Scalmsg);
}
