#include "normpage.h"
#include <QGridLayout>
#include <QDebug>

NormPage::NormPage(DataFrame* df,QWidget* parent)
{
    mode = new QComboBox;
    qcLab = new QComboBox;
    labMode = new QLabel("Mode");
    labQC = new QLabel("QC Label");

    QStringList cpClass = (df->classCol);
    cpClass.removeDuplicates();
    qcLab->insertItems(0,cpClass);

    mode->insertItem(0,"TIC");
    mode->insertItem(1,"PQN");

    qcLab->setCurrentIndex(0);
    mode->setCurrentIndex(0);

    QGridLayout *ly = new QGridLayout;
    ly->addWidget(labMode,0,0);
    ly->addWidget(mode,0,1);
    ly->addWidget(labQC,1,0);
    ly->addWidget(qcLab,1,1);

    setLayout(ly);
    setTitle("Normalisation");

    registerField("modeNorm",mode);   //here we just collect the current index since we need to pass a NormMode:: enum
    registerField("QCLabNorm",qcLab,"currentText");
}

