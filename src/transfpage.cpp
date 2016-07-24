#include "transfpage.h"
#include <QGridLayout>

TransfPage::TransfPage(DataFrame* df,QWidget *parent)
    :QWizardPage(parent)
{
    mode = new QComboBox;
    QCl = new QComboBox;
    lab = new QLabel("Transformation");
    lab_2 = new QLabel("QC label");

    QStringList cpClass = (df->classCol);
    cpClass.removeDuplicates();

    QCl->insertItems(0,cpClass);

    mode->insertItem(0,"Inverse hyperbolic sine");
    mode->insertItem(1,"Normal logarithm");
    mode->insertItem(2,"Generalised logarithm");

    QCl->setCurrentIndex(0);
    mode->setCurrentIndex(0);

    QGridLayout *ly = new QGridLayout;
    ly->addWidget(lab,0,0);
    ly->addWidget(mode,0,1);
    ly->addWidget(lab_2,1,0);
    ly->addWidget(QCl,1,1);

    setLayout(ly);
    setTitle("Transformation");

    registerField("modeTransf",mode);   //here we just collect the current index since we need to pass a NormMode:: enum
    registerField("QCLabTransf",QCl,"currentText");
}

