#ifndef SCALPAGE_H
#define SCALPAGE_H


#include <QWizardPage>
#include <QLabel>
#include <QComboBox>

class ScalPage : public QWizardPage
{
    Q_OBJECT
public:
    ScalPage(QWidget* parent=0);
private:
    QLabel *lab;
    QComboBox *mode;
};

#endif // SCALPAGE_H
