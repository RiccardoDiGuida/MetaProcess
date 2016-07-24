#ifndef TRANSFPAGE_H
#define TRANSFPAGE_H

#include <QWizardPage>
#include <QComboBox>
#include <QLabel>
#include "dataframe.h"

class TransfPage : public QWizardPage
{
    Q_OBJECT
public:
    TransfPage(DataFrame* df,QWidget* parent=0);

private:
    QComboBox* mode;
    QComboBox* QCl;
    QLabel* lab;
    QLabel* lab_2;
};

#endif // TRANSFPAGE_H
