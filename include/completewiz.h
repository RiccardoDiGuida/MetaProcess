#ifndef COMPLETEWIZ_H
#define COMPLETEWIZ_H

#include <QWizard>
#include "processdf.h"

class CompleteWiz : public QWizard
{
    Q_OBJECT
public:
    CompleteWiz(ProcessDF* df,QWidget *parent=0);

    QString Noisemsg,PeakFmsg,Normmsg,Qasmsg,Transfmsg,Scalmsg,Impmsg;

private:
    QList<bool> Operate;
    ProcessDF* DF;

public slots:
    void skip();
    void NextBut();
    void finalise();
};

#endif // COMPLETEWIZ_H
