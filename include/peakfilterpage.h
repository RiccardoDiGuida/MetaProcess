#ifndef PEAKFILTERPAGE_H
#define PEAKFILTERPAGE_H

#include <QWizardPage>
#include <QSpinBox>
#include <QLabel>

class PeakFilterPage : public QWizardPage
{
    Q_OBJECT
public:
    PeakFilterPage(QWidget* parent=0);
private:
    QSpinBox *feat,*samp;
    QLabel *featLab,*sampLab;
};

#endif // PEAKFILTERPAGE_H
