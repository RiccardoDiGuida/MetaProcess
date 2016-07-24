#include "ttestdial.h"
#include <QGridLayout>
#include <QDialogButtonBox>

ttestDial::ttestDial(AnalysisDF* df,QWidget * parent, Qt::WindowFlags f)
    :QDialog(parent,f)
{
    cb_pair = new QCheckBox("Paired test");
    cb_multcomp = new QCheckBox("Multiple comparison correction (Benjamini-Hochberg)");
    meta = new QComboBox;
    fac_1 = new QComboBox;
    fac_2 = new QComboBox;
    pair_fac = new QComboBox;
    meta_l = new QLabel("Select Metadata (factors)");
    fac1_l = new QLabel("Factor 1");
    fac2_l = new QLabel("Factor 2");
    pairfac_l = new QLabel("Pairing factors");

    QStringList metaClass;
    metaClass.append(df->className);
    metaClass.append(df->namesMetaData);
    meta->insertItems(0,metaClass);

    QStringList cpClass = (df->classCol);
    cpClass.removeDuplicates();
    fac_1->insertItems(0,cpClass);
    fac_2->insertItems(0,cpClass);

    fac_1->setCurrentIndex(0);
    fac_2->setCurrentIndex(1);

    pair_fac->setEnabled(false);
    pairfac_l->setEnabled(false);

    pair_fac->insertItems(0,metaClass);
    pair_fac->setCurrentIndex(1);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                         | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    dfRef = df;

    QGridLayout *gl = new QGridLayout;
    gl->addWidget(meta_l,0,0);
    gl->addWidget(meta,0,1);
    gl->addWidget(fac1_l,1,0);
    gl->addWidget(fac_1,1,1);
    gl->addWidget(fac2_l,2,0);
    gl->addWidget(fac_2,2,1);
    gl->addWidget(cb_pair,3,0);
    gl->addWidget(pairfac_l,4,0);
    gl->addWidget(pair_fac,4,1);
    gl->addWidget(cb_multcomp,5,0);
    gl->addWidget(buttonBox,6,1);

    setLayout(gl);

    if(df->namesMetaData.count()>0)
        connect(meta,SIGNAL(currentIndexChanged(const QString&)),this,SLOT(changefacs(const QString&)));

    connect(cb_pair,SIGNAL(stateChanged(int)),this,SLOT(togglePair(int)));
}

void ttestDial::changefacs(const QString& metaD)
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
    fac_1->insertItems(0,cpClass);
    fac_2->insertItems(0,cpClass);
    fac_1->setCurrentIndex(0);
    fac_2->setCurrentIndex(1);
}

void ttestDial::togglePair(int checkst)
{
    if(checkst)
    {
        pair_fac->setEnabled(true);
        pairfac_l->setEnabled(true);
    }
    else
    {
        pair_fac->setEnabled(false);
        pairfac_l->setEnabled(false);
    }
}
