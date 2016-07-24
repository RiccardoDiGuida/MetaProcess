#ifndef DATASETMODEL_H
#define DATASETMODEL_H
#include <QAbstractTableModel>
#include <QStringList>
#include <QChar>
#include <QFile>
#include <QItemSelectionModel>
#include <QModelIndex>

class DatasetModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    DatasetModel(QObject *parent=0);
    virtual QVariant headerData (int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    QString toString() const;
    void setSource(const QString& filename, QChar sep);
    bool readTabularFile(QFile& file,const QChar sep);

    void check();

    QList<QStringList> TotalMat;
    QStringList clsDF;
    QString clsName;
    int clsCol;
    QStringList rownames;
    QStringList colnames;
    QList<int> colMetaData; //  The index refers to TotalMat
    QList<QStringList> metaDF;
    QStringList namesMetaData;
    bool headRowUsr=false;
    bool headColUsr=false;
    QString excludedRowNames;
    QString excludedColNames;

signals:
    void columnToSelect(const QItemSelection&,QItemSelectionModel::SelectionFlags);
    void initialiseDF(DatasetModel&);

public slots:
    bool markColumn(const QModelIndex&);
    void transpose();
    bool setClassCol(int);
    void headerFirstRow(bool checkState);
    void headerFirstCol(bool checkState);
    void ClassColumn(int col);
    void finalise();
};

#endif // DATASETMODEL_H
