#ifndef NOISEFILLPAGE_H
#define NOISEFILLPAGE_H

#include <QWizardPage>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include "dataframe.h"

class NoiseFillPage : public QWizardPage
{
    Q_OBJECT
public:
    NoiseFillPage(DataFrame* df,QWidget* parent=0);

private:
    QComboBox *box;
    QLabel *lab;
    QLabel *lab_2;
    QSpinBox *sb;
};

#endif // NOISEFILLPAGE_H
