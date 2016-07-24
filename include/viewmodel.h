#ifndef VIEWMODEL_H
#define VIEWMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class ViewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ViewModel(QList<QStringList>* mat, QStringList* cnames, QStringList* rnames, QObject *parent=0);
    virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

signals:

public slots:

private:
    QStringList colnames;
    QStringList rownames;
    QList<QStringList> sourceMat;
};

#endif // VIEWMODEL_H
