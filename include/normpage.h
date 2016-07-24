#ifndef NORMPAGE_H
#define NORMPAGE_H

#include <QWizardPage>
#include <QComboBox>
#include <QLabel>
#include "dataframe.h"

class NormPage : public QWizardPage
{
    Q_OBJECT
public:
    NormPage(DataFrame* df,QWidget* parent=0);

private:
    QComboBox *mode,*qcLab;
    QLabel *labMode,*labQC;
};

#endif // NORMPAGE_H
