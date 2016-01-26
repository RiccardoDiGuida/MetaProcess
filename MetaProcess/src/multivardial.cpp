#include "multivardial.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>

multivardial::multivardial(AnalysisDF* df,QWidget * parent, Qt::WindowFlags f)
    :QDialog(parent,f)
{
    twoway = new QRadioButton("Two classes selection");
    threeway = new QRadioButton("Three classes selection");
    all = new QRadioButton("All classes");
    meta = new QComboBox;
    fac1_twow = new QComboBox;
    fac2_twow = new QComboBox;
    fac1_threew = new QComboBox;
    fac2_threew = new QComboBox;
    fac3_threew = new QComboBox;
    meta_l = new QLabel("Select Metadata (factors)");
    fac1_twow_l = new QLabel("Factor 1");
    fac2_twow_l = new QLabel("Factor 2");
    fac1_threew_l = new QLabel("Factor 1");
    fac2_threew_l = new QLabel("Factor 2");
    fac3_threew_l = new QLabel("Factor 3");
    QGroupBox* gbox = new QGroupBox;

    QVBoxLayout *vl = new QVBoxLayout;

    vl->addWidget(all);
    vl->addWidget(twoway);
    vl->addWidget(threeway);

    all->setChecked(true);
    twoway->setCheckable(false);
    threeway->setCheckable(false);

    gbox->setLayout(vl);

    QStringList metaClass;
    metaClass.append(df->className);
    metaClass.append(df->namesMetaData);
    meta->insertItems(0,metaClass);

    QStringList cpClass = (df->classCol);
    cpClass.removeDuplicates();
    if(cpClass.length()>1)
    {
        twoway->setCheckable(true);
        fac1_twow->insertItems(0,cpClass);
        fac2_twow->insertItems(0,cpClass);
        fac1_twow->setCurrentIndex(0);
        fac2_twow->setCurrentIndex(1);
    }
    if(cpClass.length()>2)
    {
        threeway->setCheckable(true);
        fac1_threew->insertItems(0,cpClass);
        fac2_threew->insertItems(0,cpClass);
        fac3_threew->insertItems(0,cpClass);
        fac1_threew->setCurrentIndex(0);
        fac2_threew->setCurrentIndex(1);
        fac3_threew->setCurrentIndex(2);
    }

    enableThreeWay(false);
    enableTwoWay(false);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    dfRef = df;

    QGridLayout *gl_min = new QGridLayout;
 /*   QLabel *p = new QLabel;
    p->setMaximumHeight(all->height());
    p->setMaximumWidth(all->width());
    gl_min->addWidget(p,0,0);*/
    gl_min->addWidget(fac1_twow_l,1,1);
    gl_min->addWidget(fac1_twow,1,2);
    gl_min->addWidget(fac2_twow_l,1,3);
    gl_min->addWidget(fac2_twow,1,4);
    gl_min->addWidget(fac1_threew_l,2,1);
    gl_min->addWidget(fac1_threew,2,2);
    gl_min->addWidget(fac2_threew_l,2,3);
    gl_min->addWidget(fac2_threew,2,4);
    gl_min->addWidget(fac3_threew_l,2,5);
    gl_min->addWidget(fac3_threew,2,6);

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(gbox);
    hl->addLayout(gl_min);

    QHBoxLayout *hl_1 = new QHBoxLayout;
    hl_1->addWidget(meta_l);
    hl_1->addWidget(meta);

    QVBoxLayout *vbl = new QVBoxLayout;
    vbl->addLayout(hl_1);
    vbl->addLayout(hl);

    vbl->addWidget(buttonBox);

    setLayout(vbl);

    if(df->namesMetaData.count()>0)
        connect(meta,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changefacs(const QString&)));

    connect(twoway,SIGNAL(toggled(bool)),this,SLOT(enableTwoWay(bool)));
    connect(threeway,SIGNAL(toggled(bool)),this,SLOT(enableThreeWay(bool)));

}

void multivardial::changefacs(const QString& metaD)
{
    QStringList cpClass;
    if(metaD==dfRef->className)
        cpClass = (dfRef->classCol);
    else
    {
        int Idx = dfRef->namesMetaData.indexOf(metaD);
        for(int i=0;i<dfRef->metaDF.count();i++)
            cpClass.append(dfRef->metaDF.at(i).at(Idx));
    }
    cpClass.removeDuplicates();
    all->setChecked(true);
    twoway->setCheckable(false);
    threeway->setCheckable(false);
    if(cpClass.length()>1)
    {
        twoway->setCheckable(true);
        fac1_twow->insertItems(0,cpClass);
        fac2_twow->insertItems(0,cpClass);
        fac1_twow->setCurrentIndex(0);
        fac2_twow->setCurrentIndex(1);
    }
    if(cpClass.length()>2)
    {
        threeway->setCheckable(true);
        fac1_threew->insertItems(0,cpClass);
        fac2_threew->insertItems(0,cpClass);
        fac3_threew->insertItems(0,cpClass);
        fac1_threew->setCurrentIndex(0);
        fac2_threew->setCurrentIndex(1);
        fac3_threew->setCurrentIndex(2);
    }
}

void multivardial::enableTwoWay(bool on)
{
    fac1_twow->setEnabled(on);
    fac2_twow->setEnabled(on);
    fac1_twow_l->setEnabled(on);
    fac2_twow_l->setEnabled(on);
}

void multivardial::enableThreeWay(bool on)
{
    fac1_threew->setEnabled(on);
    fac2_threew->setEnabled(on);
    fac3_threew->setEnabled(on);
    fac1_threew_l->setEnabled(on);
    fac2_threew_l->setEnabled(on);
    fac3_threew_l->setEnabled(on);
}
