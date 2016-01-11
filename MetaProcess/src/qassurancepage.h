#ifndef QASSURANCEPAGE_H
#define QASSURANCEPAGE_H

#include <QWizardPage>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include "dataframe.h"

class QAssurancePage : public QWizardPage
{
    Q_OBJECT
public:
    QAssurancePage(DataFrame* df,QWidget* parent=0);

signals:

public slots:

private:
    QLabel *lab,*perc;
    QComboBox *cb;
    QSpinBox *sb;
};

#endif // QASSURANCEPAGE_H
