#ifndef IMPUTATIONPAGE_H
#define IMPUTATIONPAGE_H

#include <QWizardPage>
#include <QLabel>
#include <QComboBox>

class ImputationPage : public QWizardPage
{
    Q_OBJECT
public:
    ImputationPage(QWidget* parent=0);

private:
    QLabel *lab;
    QComboBox *mode;
};

#endif // IMPUTATIONPAGE_H
