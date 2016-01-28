#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QCheckBox>

#include "ui_mainwindow.h"
#include "datasetmodel.h"
#include "mytabview.h"
#include "tabselmodel.h"
#include "analysisdf.h"
#include "analysisresult.h"
#include "multitestresult.h"

class MainWindow : public QMainWindow,private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool setTemporaryPage();
    bool eventFilter(QObject *watched, QEvent *e);

    void clearFrame();

protected:
    void setupActions();
    bool mayDiscardDocument();
    void saveFile(const QString&);

/*    void writeSettings();
    void readSettings();   */

protected slots:
    void newProject();
    void loadProject();
    void saveProject();
    void saveProjectAs();
    void exportAs();

    void exportUniStatsAs();
//    void exportMultiStatsAs();

    void viewProject();
    void viewUniStats();
    void viewMultiStats();

    void NoiseFilling();
    void PeakFiltering();
    void QAssurance();
    void Normalisation();
    void MVI();
    void Transformation();
    void Scaling();
    void Complete();

    void ttestwid();
    void PCAwid();
/*     void ANOVAwid();
    void Wilcox();
    void PCAwid();
    void PLSwid();

   void about();  */
    void test();

public slots:
    void ShowContextMenu(const QPoint&);
    void analyseErr();
    void revert();

private:
    QString mFilePath;
    DatasetModel model;
    QTableView* view;
    AnalysisDF data;
    TabSelModel* selModel;
    QCheckBox *ckb_row;
    QCheckBox *ckb_col;
    QList<AnalysisResult> res_ttest;
    MultitestResult res_multi;
    QString msg;

    int err;  //error code
};

#endif // MAINWINDOW_H
