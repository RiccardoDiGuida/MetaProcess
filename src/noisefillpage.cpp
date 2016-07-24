#include "noisefillpage.h"
#include <QGridLayout>
#include <QDebug>

NoiseFillPage::NoiseFillPage(DataFrame* df,QWidget* parent)
    : QWizardPage(parent)
{
    box = new QComboBox;
    lab = new QLabel("Metadata factor");
    lab_2 = new QLabel("% of missing values threshold to fill");
    sb = new QSpinBox;

    box->insertItem(0,(df->className));
    box->insertItems(1,(df->namesMetaData));

    sb->setMaximum(100);
    sb->setMinimum(30);

    sb->setValue(100);
    box->setCurrentIndex(0);

    QGridLayout *ly = new QGridLayout;
    ly->addWidget(box,0,0);
    ly->addWidget(lab,0,1);
    ly->addWidget(sb,1,0);
    ly->addWidget(lab_2,1,1);

    setLayout(ly);

    setTitle("Noise Filling");

 //   QObject::connect(box,static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged),[&](){factor=box->currentText();});
  //  QObject::connect(sb,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),[&](){percent=sb->value();});
    registerField("factor",box,"currentText");
    registerField("percent",sb);
}
