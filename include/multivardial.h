#ifndef MULTIVARDIAL_H
#define MULTIVARDIAL_H

#include <QDialog>
#include <QRadioButton>
#include <QLabel>
#include <QComboBox>
#include "analysisdf.h"

class multivardial : public QDialog
{
    Q_OBJECT
public:
    multivardial(AnalysisDF* df,QWidget * parent = 0, Qt::WindowFlags f = 0);
    QRadioButton *twoway,*threeway,*all;
    QComboBox *meta,*fac1_twow,*fac2_twow,*fac1_threew,*fac2_threew,*fac3_threew;
    QLabel *meta_l,*fac1_twow_l,*fac2_twow_l,*fac1_threew_l,*fac2_threew_l,*fac3_threew_l;

public slots:
    void changefacs(const QString& metaD);
    void enableTwoWay(bool on);
    void enableThreeWay(bool on);

private:
    AnalysisDF* dfRef;
};

#endif // MULTIVARDIAL_H
