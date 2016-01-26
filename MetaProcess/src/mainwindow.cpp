#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "noisefillpage.h"
#include "peakfilterpage.h"
#include "qassurancepage.h"
#include "normpage.h"
#include "imputationpage.h"
#include "transfpage.h"
#include "scalpage.h"
#include "completewiz.h"
#include "viewmodel.h"
#include "ttestdial.h"
#include "multivardial.h"
#include <QSplitter>
#include <QTableView>
#include <QBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QLabel>
#include <QDebug>
#include <QFileDialog>
#include <QMouseEvent>
#include <QWizard>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    setupActions();

    MainSplitter->setSizes(QList<int>{600,300});

    MainSplitter->setCollapsible(1,false);

    view = new QTableView;
    view->installEventFilter(this);

    QGridLayout* gl = new QGridLayout;
    frame->setLayout(gl);

    actionSave_Project->setEnabled(false);
    actionSave_Project_As->setEnabled(false);
    actionExport_as->setEnabled(false);
    actionExport_image_as->setEnabled(false);
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupActions()
{
    connect(actionQuit, SIGNAL(triggered(bool)),
            qApp, SLOT(quit()));
    connect(actionNew_Project, SIGNAL(triggered(bool)),
            this,SLOT(newProject()));
    connect(actionOpen_Project, SIGNAL(triggered(bool)),
            this, SLOT(loadProject()));
    connect(actionSave_Project, SIGNAL(triggered(bool)),
            this, SLOT(saveProject()));
    connect(actionSave_Project_As, SIGNAL(triggered(bool)),
            this, SLOT(saveProjectAs()));
    connect(actionExport_as,SIGNAL(triggered(bool)),
            this, SLOT(exportAs()));

    connect(actionShow_current_project,SIGNAL(triggered(bool)),
            this,SLOT(viewProject()));

    connect(actionNoise_Filling,SIGNAL(triggered(bool)),
            this,SLOT(NoiseFilling()));
    connect(actionPeak_Filtering,SIGNAL(triggered(bool)),
            this,SLOT(PeakFiltering()));
    connect(actionQuality_Assurance,SIGNAL(triggered(bool)),
            this,SLOT(QAssurance()));
    connect(actionNormalisation,SIGNAL(triggered(bool)),
            this,SLOT(Normalisation()));
    connect(actionMissing_Values_Imputation,SIGNAL(triggered(bool)),
            this,SLOT(MVI()));
    connect(actionTransformation,SIGNAL(triggered(bool)),
            this,SLOT(Transformation()));
    connect(actionScaling,SIGNAL(triggered(bool)),
            this,SLOT(Scaling()));
    connect(actionComplete,SIGNAL(triggered(bool)),
            this,SLOT(Complete()));

    connect(actionT_test,SIGNAL(triggered(bool)),
            this,SLOT(ttestwid()));
    connect(actionMann_Whitney_U_test,SIGNAL(triggered(bool)),
            this,SLOT(ttestwid()));
    connect(actionPCA_1,SIGNAL(triggered(bool)),
            this,SLOT(PCAwid()));
}

void MainWindow::newProject()
{
    mFilePath = "";
    if(setTemporaryPage())
    {
        actionSave_Project->setEnabled(true);
        actionSave_Project_As->setEnabled(true);
        actionExport_as->setEnabled(true);
    }
}

bool MainWindow::setTemporaryPage()
{
    QWidget *w = new QWidget();

    QGridLayout *gl = new QGridLayout();
    gl->addWidget(w);

    QPushButton *but = new QPushButton("Transpose",w);
    ckb_row = new QCheckBox("Header in first row",w);
    ckb_col = new QCheckBox("Header in first col",w);
    QLabel *lab = new QLabel("Column contatining classes",w);
    QSpinBox *spin = new QSpinBox(w);
    spin->setMinimum(1);
    QPushButton *finBut = new QPushButton("Done",w);

    QString filename = QFileDialog::getOpenFileName(this,tr("Open new project"),"C:/","Comma Separated Values(*.csv);;Tabular file(*.tab);;Text file(*.txt)");

    if(filename.isEmpty()) return false;

    model.setSource(filename,',');

    selModel = new TabSelModel(&model);

  //  view = new QTableView;

    view->setModel(&model);
    view->setSelectionModel(selModel);

    QHBoxLayout *lay = new QHBoxLayout(w);
    QHBoxLayout *seclay = new QHBoxLayout(w);
    QVBoxLayout *vlay = new QVBoxLayout(w);

    seclay->addWidget(lab);
    seclay->addWidget(spin);
    vlay->addWidget(but);
    vlay->addWidget(ckb_row);
    vlay->addWidget(ckb_col);
    vlay->addLayout(seclay);
    vlay->addWidget(finBut);
    lay->addWidget(view);
    lay->addLayout(vlay);

    QObject::connect(spin,SIGNAL(valueChanged(int)),&model,SLOT(ClassColumn(int)));
    QObject::connect(&model,SIGNAL(columnToSelect(const QItemSelection&,QItemSelectionModel::SelectionFlags)),
                     selModel,SLOT(select(const QItemSelection&,QItemSelectionModel::SelectionFlags)));
    QObject::connect(but,SIGNAL(clicked(bool)),&model,SLOT(transpose()));
    QObject::connect(selModel,SIGNAL(currentChanged(QModelIndex,QModelIndex)),selModel,SLOT(CheckSelection(QModelIndex)));
    QObject::connect(but,SIGNAL(clicked()),this,SLOT(revert()));
    QObject::connect(ckb_row,SIGNAL(clicked(bool)),&model,SLOT(headerFirstRow(bool)));
    QObject::connect(ckb_col,SIGNAL(clicked(bool)),&model,SLOT(headerFirstCol(bool)));
    QObject::connect(finBut,SIGNAL(clicked(bool)),&model,SLOT(finalise()));
    QObject::connect(&model,SIGNAL(initialiseDF(DatasetModel&)),&data,SLOT(setModel(DatasetModel&)));
    QObject::connect(&data,SIGNAL(modelSet()),w,SLOT(close()));

    view->setContextMenuPolicy(Qt::CustomContextMenu);
//    QObject::connect(view, SIGNAL(customContextMenuRequested(const QPoint&)),
//        this, SLOT(ShowContextMenu(const QPoint&)));

    //QObject::connect(selModel,&QItemSelectionModel::currentChanged,[&](){lay->addWidget(but);});

    delete frame->layout(); // delete previous layout
    frame->setLayout(gl);
    w->show();

    return true;
}

void MainWindow::saveProject()
{
    if(mFilePath.isEmpty())
        saveProjectAs();
    else
        saveFile(mFilePath);
}

void MainWindow::saveProjectAs()
{
    mFilePath = QFileDialog::getSaveFileName(this,tr("Save File"),"C:/","XML files(*.xml)");
        if(mFilePath.isEmpty())
            return;
        saveFile(mFilePath);
}

void MainWindow::saveFile(const QString& name)
{
    QFile file(name);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        data.writeToXML(&file);
        QMainWindow::statusBar()->showMessage(tr("File saved successfully."), 3000);
    }
}

void MainWindow::loadProject()
{
    QString filename = QFileDialog::getOpenFileName(this,tr("Open project File"),"C:/","XML files(*.xml)");
    if(filename.isEmpty())
        return;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        data.readFromXML(&file);
        mFilePath = filename;
        QMainWindow::statusBar()->showMessage(tr("File successfully loaded."), 3000);
    }

    actionSave_Project->setEnabled(true);
    actionSave_Project_As->setEnabled(true);
    actionExport_as->setEnabled(true);
}

void MainWindow::exportAs()
{
    QString expFPath = QFileDialog::getSaveFileName(this,tr("Save File"),"C:/","Comma Separated Values(*.csv);;Tabular file(*.tab);;Text file(*.txt)");
    QFile file(expFPath);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        data.exportTo(&file);
        QMainWindow::statusBar()->showMessage(tr("File successfully exported."), 3000);
    }
}

void MainWindow::exportUniStatsAs()
{
    QString expFPath = QFileDialog::getSaveFileName(this,tr("Save File"),"C:/","Comma Separated Values(*.csv);;Tabular file(*.tab);;Text file(*.txt)");
    QFile file(expFPath);
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        data.PrintTwoSampleTest(res_ttest,&file);
        QMainWindow::statusBar()->showMessage(tr("File successfully exported."), 3000);
    }
}

void MainWindow::viewProject()
{
    QTableView* tview = new QTableView;
    QTableView* tview_2 = new QTableView;

    QList<QStringList>* mat_meta = new QList<QStringList>;
    QList<QStringList>* mat_num = new QList<QStringList>;

    for(int i=0;i<data.classCol.count();i++)
    {
        QStringList RowMeta;
        RowMeta.append(data.classCol.at(i));
        for(int j=0;j<data.namesMetaData.count();j++)
            RowMeta.append(data.metaDF.at(j));

        mat_meta->append(RowMeta);
        mat_num->append(data.rowToString(i));
    }
    QStringList cnamMet = data.namesMetaData;
    cnamMet.prepend(data.className);
    ViewModel* mod_meta = new ViewModel(mat_meta,&cnamMet,&data.rnames);
    ViewModel* mod_num = new ViewModel(mat_num,&data.cnamesNum,&data.rnames);

    tview->setModel(mod_meta);
    tview_2->setModel(mod_num);
    QHBoxLayout* gl = new QHBoxLayout;
    gl->addWidget(tview);
    gl->addWidget(tview_2);

    connect(tview->verticalScrollBar(),SIGNAL(valueChanged(int)),
            tview_2->verticalScrollBar(),SLOT(setValue(int)));

    connect(tview_2->verticalScrollBar(),SIGNAL(valueChanged(int)),
            tview->verticalScrollBar(),SLOT(setValue(int)));

    clearFrame();   // delete previous layout
    frame->setLayout(gl);
}

void MainWindow::viewUniStats()
{
    QTableView* tview = new QTableView;
    QPushButton *but = new QPushButton("Export Results Table As...");

    QList<QStringList>* ResMat = new QList<QStringList>;

    for(uint i=0;i<res_ttest.count();i++)
    {
        QStringList row;
        row.append(QString::number(res_ttest[i].p_val));
        row.append(QString::number(res_ttest[i].FC));
        QStringList tmp({QString::number(res_ttest[i].conf_int.first),QString::number(res_ttest[i].conf_int.second)});
        row.append(tmp.join("/"));

        ResMat->append(row);
    }
    QStringList cnam;
    cnam << "p-value" << "FC" << "CI";
    ViewModel* statmod = new ViewModel(ResMat,&cnam,&data.cnamesNum);

    tview->setModel(statmod);
    QHBoxLayout* gl= new QHBoxLayout;
    gl->addWidget(tview);
    gl->addWidget(but);

    connect(but,SIGNAL(clicked(bool)),SLOT(exportUniStatsAs()));

    clearFrame();   // delete previous layout
    frame->setLayout(gl);
}

void MainWindow::NoiseFilling()
{
    QWizard *wiz = new QWizard;
    NoiseFillPage *pg = new NoiseFillPage(&data);
    wiz->addPage(pg);
    wiz->setWindowTitle("Noise Filling");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);
    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){data.NoiseFill(wiz->field("percent").toInt(),wiz->field("factor").toString(),msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset noise filled.\n");});

    clearFrame();   // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::PeakFiltering()
{
    QWizard *wiz = new QWizard;
    PeakFilterPage *pg = new PeakFilterPage;
    wiz->addPage(pg);
    wiz->setWindowTitle("Peak Filtering");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);
    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){data.PeakFilter(wiz->field("percFeat").toInt(),wiz->field("percSamp").toInt(),true,msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset filtered.\n");});

    clearFrame();  // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::QAssurance()
{
    QWizard *wiz = new QWizard;
    QAssurancePage *pg = new QAssurancePage(&data);
    wiz->addPage(pg);
    wiz->setWindowTitle("Quality assurance");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);
    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){err = data.QAssurance(wiz->field("QCLabQas").toString(),wiz->field("percRSD").toInt(),msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset quality checked.\n");});
    connect(wiz,SIGNAL(accepted()),SLOT(analyseErr()));


    clearFrame();   // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::Normalisation()
{
    QWizard *wiz = new QWizard;
    NormPage *pg = new NormPage(&data);
    wiz->addPage(pg);
    wiz->setWindowTitle("Normalisation");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);
    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){data.Normalisation(static_cast<ProcessDF::NormMode>(wiz->field("modeNorm").toInt()),wiz->field("QCLabNorm").toString(),msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset normalised.\n");});

    clearFrame();  // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::Transformation()
{
    QWizard *wiz = new QWizard;
    TransfPage *pg = new TransfPage(&data);

    wiz->addPage(pg);
    wiz->setWindowTitle("Transformation");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);
    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){data.Transform(static_cast<ProcessDF::TransformMode>(wiz->field("modeTransf").toInt()),wiz->field("QCLabTransf").toString(),msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset Transformed.\n");});

    clearFrame();   // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::Scaling()
{
    QWizard *wiz = new QWizard;
    ScalPage *pg = new ScalPage();
    wiz->addPage(pg);
    wiz->setWindowTitle("Scaling");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);
    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){data.Scale(static_cast<ProcessDF::ScalingMode>(wiz->field("modeScal").toInt()),msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset scaled.\n");});

    clearFrame();   // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::MVI()
{
    QWizard *wiz = new QWizard;
    ImputationPage *pg = new ImputationPage();
    wiz->addPage(pg);
    wiz->setWindowTitle("Missing Value Imputation");
    wiz->setOption(QWizard::NoBackButtonOnStartPage);

    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);

    QString msg;

    connect(wiz,&QDialog::accepted,[=,&msg](){data.MVImpute(static_cast<ProcessDF::MVIMode>(wiz->field("modeMVI").toInt()),msg);});
    connect(wiz,&QDialog::accepted,[&](){textEdit->append("Dataset imputed.\n");});

    clearFrame();   // delete previous layout
    frame->setLayout(ly);
}

void MainWindow::Complete()
{
    CompleteWiz* wiz = new CompleteWiz(&data);
    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(wiz);
    ly->addWidget(pb);

    clearFrame();
    frame->setLayout(ly);
}

void MainWindow::ttestwid()
{
    ttestDial* dbox= new ttestDial(&data);
    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(dbox);

    res_ttest.clear();

    connect(dbox,&QDialog::accepted,[=,this](){data.ttest(res_ttest,dbox->meta->currentText(),dbox->fac_1->currentText(),
                                                         dbox->fac_2->currentText(),dbox->cb_pair->isChecked(),dbox->cb_multcomp->isChecked(),
                                                         msg,dbox->pair_fac->currentText());});
    connect(&data,&AnalysisDF::computationUniStatsDone,[this](){textEdit->append(msg);});
    connect(&data,SIGNAL(computationUniStatsDone()),SLOT(viewUniStats()));

    clearFrame();
    frame->setLayout(ly);
}

void MainWindow::PCAwid()
{
    multivardial* dbox=new multivardial(&data);
    QVBoxLayout *ly = new QVBoxLayout;
    QLabel *pb = new QLabel;
    ly->addWidget(dbox);

    clearFrame();
    frame->setLayout(ly);
}

void MainWindow::test()
{
 //   qDebug() << percent << factor;
}

void MainWindow::ShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Add to metadata");

    QAction* selectedItem = myMenu.exec(globalPos);
    if(selectedItem)
    {
        model.markColumn(selModel->ColSelected.second);
    }
}

void MainWindow::revert()
{
    auto temp = ckb_col->checkState();
    ckb_col->setCheckState(ckb_row->checkState());
    ckb_row->setCheckState(temp);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *e)
{
    if(watched==view && e->type()==QEvent::MouseButtonPress)
    {
//        QModelIndexList idxl = view->selectedIndexes();
        QMouseEvent *mou = static_cast<QMouseEvent*>(e);
        if(mou->button()==Qt::RightButton && selModel->ColSelected.first)
        {
            ShowContextMenu(mou->pos());
            return true;
        }
    }
    return QMainWindow::eventFilter(watched,e);
}

void MainWindow::analyseErr()
{
    switch(err)
    {
        case 0:
            break;
        case 1:
            textEdit->append("No peaks are left after filtering!\n");
            break;
        case 2:
            textEdit->append("Only one peak left after filtering!\n");
            break;
        case 3:
            textEdit->append("Lambda tending to infinity! Using standard\n");
            break;
        case 4:
            textEdit->append("Lambda tending to -infinity! Using standard\n");
            break;
    }
}

void MainWindow::clearFrame()
{
    QLayoutItem * item;
    QLayout * sublayout;
    QWidget * widget;

    while ((item = frame->layout()->takeAt(0))!=0) {
        if ((sublayout = item->layout()) != 0) {/* do the same for sublayout*/}
        else if ((widget = item->widget()) != 0) {widget->hide(); delete widget;}
        else {delete item;}
    }

    // then finally
    delete frame->layout();
}
