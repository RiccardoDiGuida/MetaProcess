#include "scalpage.h"
#include <QHBoxLayout>

ScalPage::ScalPage(QWidget* parent)
    :QWizardPage(parent)
{
    mode = new QComboBox;
    lab = new QLabel("Mode");

    mode->insertItem(0,"Pareto scaling");
    mode->insertItem(1,"Range scaling");
    mode->insertItem(2,"Autoscaling");

    mode->setCurrentIndex(0);

    QHBoxLayout *ly = new QHBoxLayout;
    ly->addWidget(lab);
    ly->addWidget(mode);

    setLayout(ly);
    setTitle("Scaling");

    registerField("modeScal",mode);   //here we just collect the current index since we need to pass a ScalingMode:: enum
}

