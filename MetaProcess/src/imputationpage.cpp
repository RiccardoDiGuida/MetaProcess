#include "imputationpage.h"
#include <QHBoxLayout>

ImputationPage::ImputationPage(QWidget* parent)
    :QWizardPage(parent)
{
    mode = new QComboBox;
    lab = new QLabel("Mode");

    mode->insertItem(0,"Small Value Replacement");
    mode->insertItem(1,"Mean Replacement");
    mode->insertItem(2,"Median Replacement");
    mode->insertItem(3,"KNN Replacement");
//    mode->insertItem(4,"BPCA Replacement");  //NOT FOR NOW

    mode->setCurrentIndex(0);

    QHBoxLayout *ly = new QHBoxLayout;
    ly->addWidget(lab);
    ly->addWidget(mode);

    setLayout(ly);
    setTitle("Missing Value Imputation");

    registerField("modeMVI",mode);   //here we just collect the current index since we need to pass a MVIMode:: enum
}

