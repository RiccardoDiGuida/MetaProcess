#ifndef XMLHANDLER_H
#define XMLHANDLER_H

#include <QXmlStreamWriter>
#include "dataframe.h"

class XMLHandler : public QObject
{
    Q_OBJECT
public:
    XMLHandler(DataFrame& DF,QObject* parent=0);   //constr for writing
    XMLHandler(QObject* parent=0);             //constr for reading

signals:

public slots:
    bool write(QIODevice* device);
//    DataFrame read(QString filename);

protected:
    DataFrame* xmlDF;
};

#endif // XMLHANDLER_H
