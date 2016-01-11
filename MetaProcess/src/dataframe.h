#ifndef DATAFRAME_H
#define DATAFRAME_H

#include "datasetmodel.h"
#include <armadillo>

using namespace arma;

class DataFrame : public QObject
{
    Q_OBJECT
public:
    DataFrame(QObject* parent=0);
    DataFrame(const DataFrame& other);
    DataFrame& operator = (const DataFrame& other);
    void buildNumericDataframe(QList<QStringList>& sourceMat,bool colUsr,bool RowUsr,QList<int>& MetaCol,int ClassCol);
    QString colToString(int col);
    QStringList rowToString(int row);
    void colFromString(QString str,int col);

    bool writeToXML(QIODevice* device);
    bool readFromXML(QIODevice* device);

    void exportTo(QFile* file);

//protected:
    mat numDF;
    QStringList rnames;   //rownames
    QStringList cnames;   //colnames
    QStringList cnamesNum;      //colnames of the numeric matrix only
    QStringList classCol;   //class column
    QString className;
    QList<QStringList> metaDF;  // Dataframe of metadata. Class is not included
    QStringList namesMetaData;

signals:
    void modelSet();

public slots:
    void setModel(DatasetModel&);
};

#endif // DATAFRAME_H
