#include "peakfilterpage.h"
#include <QGridLayout>

PeakFilterPage::PeakFilterPage(QWidget* parent)
    : QWizardPage(parent)
{
    feat = new QSpinBox;
    samp = new QSpinBox;
    featLab = new QLabel("Threshold (%) of missing values per feature\nAbove this threshold a feature is removed");
    sampLab = new QLabel("Threshold (%) of missing values per sample\nAbove this threshold a sample is removed");

    feat->setMaximum(100);
    samp->setMaximum(100);

    feat->setMinimum(0);
    samp->setMinimum(0);

    feat->setValue(30);
    samp->setValue(100);

    QGridLayout *ly = new QGridLayout;
    ly->addWidget(featLab,0,0);
    ly->addWidget(feat,0,1);
    ly->addWidget(sampLab,1,0);
    ly->addWidget(samp,1,1);

    setLayout(ly);
    setTitle("Peak Filtering");

    registerField("percFeat",feat);
    registerField("percSamp",samp);
}

