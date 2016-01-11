#ifndef TTESTDIAL_H
#define TTESTDIAL_H

#include <QDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include "analysisdf.h"

class ttestDial : public QDialog
{
    Q_OBJECT
public:
    ttestDial(AnalysisDF* df,QWidget * parent = 0, Qt::WindowFlags f = 0);

    QCheckBox* cb_pair,*cb_multcomp;
    QComboBox *meta,*fac_1,*fac_2,*pair_fac;
    QLabel *meta_l,*fac1_l,*fac2_l,*pairfac_l;

public slots:
    void changefacs(const QString& metaD);
    void togglePair(int checkst);

private:
    AnalysisDF* dfRef;
};

#endif // TTESTDIAL_H
