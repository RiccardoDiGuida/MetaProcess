#include "qassurancepage.h"
#include <QGridLayout>

QAssurancePage::QAssurancePage(DataFrame* df,QWidget *parent)
    :QWizardPage(parent)
{
    cb = new QComboBox;
    sb = new QSpinBox;
    lab = new QLabel("QC Label");
    perc = new QLabel("RSD threshold (%)\nAbove this a feature is removed");

    QStringList cpClass = (df->classCol);
    cpClass.removeDuplicates();

    cb->insertItems(0,cpClass);

    sb->setMaximum(100);
    sb->setMinimum(1);

    sb->setValue(30);
    cb->setCurrentIndex(0);

    QGridLayout *ly = new QGridLayout;
    ly->addWidget(lab,0,0);
    ly->addWidget(cb,0,1);
    ly->addWidget(perc,1,0);
    ly->addWidget(sb,1,1);

    setLayout(ly);
    setTitle("Quality Assurance");

    registerField("percRSD",sb);
    registerField("QCLabQas",cb,"currentText");
}

