#ifndef TABSELMODEL_H
#define TABSELMODEL_H

#include <QItemSelectionModel>
#include <QPair>


class TabSelModel : public QItemSelectionModel
{
    Q_OBJECT
public:
    TabSelModel(QAbstractItemModel * model = 0);
    QPair<bool,QModelIndex> ColSelected;

signals:
   // void Column_Selected(const QModelIndex&);

public slots:
    void CheckSelection(const QModelIndex&);
};

#endif // TABSELMODEL_H
