#include "datasetmodel.h"
#include <QFile>
#include <QDebug>

DatasetModel::DatasetModel(QObject *parent)
    :QAbstractTableModel(parent)
{

}

void DatasetModel::setSource(const QString& filename, QChar sep)
{
    QFile file(filename);

    QString str = filename.section('.',-1);

    if(str=="csv"||str=="tab"||str=="txt")
        readTabularFile(file,sep);   //CHECK IF SEP IS VALID ON MAINWINDOW AND IF FILE EXISTS
    else if(str=="xml")
    {}
    else
        qDebug() << "File not recognised.";

   // qDebug() << TotalMat[3];
    clsCol = 1;   // by default the first column contains classes
}

int DatasetModel::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return TotalMat.count()-(headColUsr);  //last line is empty
}

int DatasetModel::columnCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);
    return TotalMat.at(0).count()-headRowUsr;
}

QVariant DatasetModel::headerData(int section,
            Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
            return QVariant();

    if (orientation == Qt::Horizontal)
        if(headColUsr)
            return colnames[section];
        else
            return QString("Feature %1").arg(section+1);
    else
        if(headRowUsr)
            return rownames[section];  //headColUsr evaluates to 1 or to 0
        else
            return QString("Sample_%1").arg(section+1);
}

QVariant DatasetModel::data(const QModelIndex &index,
                        int role) const
{
  if (!index.isValid()) return QVariant();

  QStringList dataRecord;
  dataRecord = TotalMat.at(index.row()+headColUsr);

  if (role == Qt::DisplayRole || role == Qt::EditRole)
    return dataRecord.at(index.column()+headRowUsr);


  return QVariant();
}

Qt::ItemFlags DatasetModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool DatasetModel::setData(const QModelIndex & index,
                const QVariant& value, int role)
{
  if (index.isValid() && (role == Qt::EditRole ||
                          role == Qt::DisplayRole)) {
    TotalMat[index.row()][index.column()] = value.toString();
    emit dataChanged(index, index);
    return true;
  }
  return false;
}

bool DatasetModel::insertRows(int row, int count,
                    const QModelIndex & parent)
{
  Q_UNUSED(parent);
  QStringList emptyRecord;
  for (int i=0; i<columnCount(QModelIndex()); i++)
    emptyRecord.append(QString());
  beginInsertRows(QModelIndex(), row, row+count-1);
  for (int i=0; i<count; i++)
    TotalMat.insert(row, emptyRecord);
  endInsertRows();

  return true;
}

bool DatasetModel::removeRows(int row, int count,
                    const QModelIndex& parent)
{
  Q_UNUSED(parent);
  if (row-count > TotalMat.count()) return false;
  beginRemoveRows(QModelIndex(), row, row+count);
  for(int i=0; i<count; i++)
    TotalMat.removeAt(row);
  endRemoveRows();
  return true;
}

QString DatasetModel::toString() const
{
   QString ab;
   foreach (QStringList record, TotalMat) {
     ab += "\"";
     record.join("\",\"");
     ab += "\"\n";
   }
   return ab;
}

bool DatasetModel::readTabularFile(QFile& file,const QChar sep)
{
    if ( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
        return false;

    QString data = QString::fromUtf8(file.readAll());

    QStringList records = data.split('\n');

//    QString firstline = records[0];
//    int nCol = firstline.split(",").count();  //number of Columns taken from first line

//    NumDF.set_size(1,nCol);

    foreach(QString record, records)
    {
       QStringList items = record.split(sep);

       TotalMat.append(items);
    }

    if(TotalMat.last().last().isEmpty())
        TotalMat.removeLast();

    return true;
}

void DatasetModel::check()
{
    qDebug() << rowCount();
    qDebug() << columnCount();

}

bool DatasetModel::markColumn(const QModelIndex& mod)
{
    if(!mod.isValid()) return false;
    else
        colMetaData.append(mod.column()+headRowUsr);
    return true;
}

void DatasetModel::transpose()
{
    emit beginResetModel();

    QList<QStringList> NewMat;

    int Nrow_ori = TotalMat.count();
    int Ncol_ori = TotalMat.at(0).count();

    for(int j=0;j<Ncol_ori;j++)
    {
        QStringList addingVector;
        for(int i=0;i<Nrow_ori;i++)
            addingVector.append(TotalMat[i][j]);

        NewMat.append(addingVector);
    }

    TotalMat = NewMat;

//    TotalMat.append(QStringList());  // add fake row so that it simulates the original condition

    auto temp = headRowUsr;
    headRowUsr = headColUsr;
    headColUsr = temp;

    auto temp_2 = colnames;
    colnames = rownames;
    rownames = temp_2;

    emit endResetModel();
}

bool DatasetModel::setClassCol(int col)
{
    if(TotalMat[1][col].isEmpty())  //fix later. find better method to understand if class is empty
        return false;
    else
        for(int i=0;i<rowCount();i++)
            clsDF.append(TotalMat[i+headColUsr][col]);
    return true;
}

void DatasetModel::headerFirstRow(bool checkState)
{
    emit beginResetModel();

    if(!checkState)
    {
        for(int i=0;i<columnCount();i++)
            colnames.append(QString::number(i));
        headColUsr=false;
        rownames.prepend(excludedRowNames);
    }
    else
    {
        colnames = TotalMat[0];
        headColUsr=true;
        if(headRowUsr)
        {
            colnames.removeFirst(); //it is a blank cell
            excludedRowNames = rownames.takeFirst(); // same goes here
        }
    }
    emit endResetModel();
}

void DatasetModel::headerFirstCol(bool checkState)
{
    emit beginResetModel();

    if(!checkState)
    {
        for(int i=0;i<rowCount();i++)
            rownames[i] = QString("Sample_%1").arg(i);
        headRowUsr=false;
        colnames.prepend(excludedColNames);
    }
    else
    {
        rownames = QStringList();
        for(int i=0;i<TotalMat.count();i++)
            rownames.append(TotalMat[i][0]);
        headRowUsr=true;

        if(headColUsr)
        {
            rownames.removeFirst();
            excludedColNames = colnames.takeFirst();
        }
    }

    emit endResetModel();
}

void DatasetModel::ClassColumn(int col)
{
    QModelIndex ind = index(0,col-1,QModelIndex()); //row does not matter since we select columns
    QItemSelection colSel(ind,ind);
    emit columnToSelect(colSel,QItemSelectionModel::Select | QItemSelectionModel::Columns | QItemSelectionModel::Current);

    clsCol=col;
}

void DatasetModel::finalise()
{
    for(int i=0;i<TotalMat.count();i++)               // CLASS DATASET
            clsDF.append(TotalMat[i][clsCol-(1-headRowUsr)]);

        if(headColUsr && !clsDF[0].isEmpty()) // header exist and for class must be different from empty string
            clsName = clsDF.takeFirst();
        else
            clsName = "Class";

    if(colMetaData.isEmpty())
        metaDF = QList<QStringList>();
    else
        for(int i=headColUsr;i<TotalMat.count();i++)
        {
            foreach(int x,colMetaData)
            {
                metaDF[i-headColUsr].append(TotalMat[i][x]);
            }
        }
    //ADD METANAMES

    emit initialiseDF(*this);
}
