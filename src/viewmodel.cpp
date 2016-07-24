#include "viewmodel.h"
#include <QDebug>

ViewModel::ViewModel(QList<QStringList>* mat, QStringList* cnames, QStringList* rnames,QObject *parent)
    :QAbstractTableModel(parent)
{
    if(mat->count()!=rnames->count() || mat->at(0).count()!=cnames->count())
        return;

    sourceMat = *mat;
    colnames = *cnames;
    rownames = *rnames;
}

int ViewModel::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return sourceMat.count();  //last line is empty
}

int ViewModel::columnCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return sourceMat.at(0).count();
}

QVariant ViewModel::headerData(int section,
            Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();

    if (orientation == Qt::Horizontal)
        return colnames.at(section);
    else
        return rownames.at(section);
}

QVariant ViewModel::data(const QModelIndex &index,
                        int role) const
{
    if (!index.isValid()) return QVariant();
    QStringList dataRecord;
    dataRecord = sourceMat.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    return dataRecord.at(index.column());

    return QVariant();
}

