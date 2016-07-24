#include "dataframe.h"
#include <QDebug>
#include <vector>

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include <QTextStream>


DataFrame::DataFrame(QObject* parent)
    :QObject(parent)
{

}

DataFrame::DataFrame(const DataFrame& other)
{
    rnames = other.rnames;
    cnames = other.cnames;
    classCol = other.classCol;
    className = other.className;
    metaDF = other.metaDF;
    namesMetaData = other.namesMetaData;
    numDF = other.numDF;
}

DataFrame& DataFrame::operator =(const DataFrame& other)
{
    rnames = other.rnames;
    cnames = other.cnames;
    classCol = other.classCol;
    className = other.className;
    metaDF = other.metaDF;
    namesMetaData = other.namesMetaData;
    numDF = other.numDF;

    return *this;
}

void DataFrame::setModel(DatasetModel& modelObj)
{
    rnames = modelObj.rownames;
    cnames = modelObj.colnames;

    classCol = modelObj.clsDF;
    className = modelObj.clsName;

    this->metaDF = modelObj.metaDF;     //CHANGE TO VALUE SO THAT DOES NOT DEPEND ON MODEL ANYMORE?
    this->namesMetaData = modelObj.namesMetaData;

    buildNumericDataframe(modelObj.TotalMat,modelObj.headColUsr,modelObj.headRowUsr,modelObj.colMetaData,modelObj.clsCol);

    //qDebug() << *classCol << '\n' << *className;

    emit modelSet();
}


void DataFrame::buildNumericDataframe(QList<QStringList>& sourceMat,bool colUsr,bool RowUsr,QList<int>& MetaCol,int ClassCol)
{
    int nFeatures = cnames.count()-(1+MetaCol.count());   //1 is the class
    numDF.set_size(1,nFeatures);

    if(colUsr)
        sourceMat.removeFirst();

    QList<int> elim;
    if(RowUsr)
        elim.append(0);
    elim.append(ClassCol+RowUsr-1);
    foreach(int x,MetaCol)
        elim.append(x); //Now we have a vector of all columns to avoid from original matrix

    QList<int> includeCol;
    for(int i=0;i<cnames.count();i++)
        includeCol.append(i+RowUsr);

    foreach(int x,elim)
        includeCol.removeAll(x);

    foreach(int x,includeCol)
        cnamesNum.append(cnames.at(x-RowUsr));

    bool nona;    // Not a NA

    for(int i=0;i<sourceMat.count();i++)
    {
        std::vector<double> vecrow;

        foreach(int col,includeCol)
        {
            double valueToInsert = sourceMat[i][col].toDouble(&nona);

            if(nona)        //if not a NA
                vecrow.push_back(valueToInsert);
            else
                vecrow.push_back(NAN);
        }
        rowvec A(vecrow);

        numDF.insert_rows(i,A);

        vecrow.resize(0);
    }

    numDF.shed_row(numDF.n_rows-1);
    qDebug() << numDF.n_cols << " " << numDF.n_rows;
}

QString DataFrame::colToString(int col)
{
    vec column = numDF.col(col);
    QStringList out;
    for(int i=0;i<column.n_elem;i++)
        out.append(QString::number(column(i)));

    return out.join(",");
}

QStringList DataFrame::rowToString(int row)
{
    rowvec rowv = numDF.row(row);
    QStringList out;
    for(int i=0;i<rowv.n_elem;i++)
        out.append(QString::number(rowv(i)));

    return out;
}

void DataFrame::colFromString(QString str,int col)
{
    QStringList colStr = str.split(",");
    bool nona;
    for(int i=0;i<colStr.length();i++)
    {
        double valueToInsert = colStr[i].toDouble(&nona);

        if(nona)
            numDF(i,col) = valueToInsert;
        else
            numDF(i,col) = NAN;
    }
}

bool DataFrame::writeToXML(QIODevice* device)
{
    if(!device)
        return false;
    QXmlStreamWriter stream(device);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("metaxml");
    stream.writeAttribute("version","0.1");
        stream.writeStartElement("metadata");
            stream.writeStartElement("section");
            stream.writeAttribute("type","string");
            stream.writeAttribute("purity","class");
            stream.writeAttribute("size",QString("[%1,1]").arg(classCol.count()));
            stream.writeAttribute("id",className);
            stream.writeCharacters(classCol.join(","));
            stream.writeEndElement();
            if(!metaDF.isEmpty())
                for(int i=0;i<metaDF[0].count();i++)
                {
                    stream.writeStartElement("section");
                    stream.writeAttribute("type","string");
                    stream.writeAttribute("purity","meta");
                    stream.writeAttribute("size",QString("[%1,1]").arg(classCol.count()));
                    stream.writeAttribute("id",namesMetaData.at(i));
                    stream.writeCharacters(classCol.join(","));
                    stream.writeEndElement();
                }
        stream.writeEndElement();
        stream.writeStartElement("data");
        stream.writeAttribute("size",QString("[%1,%2]").arg(numDF.n_rows).arg(numDF.n_cols));
            for(int i=0;i<numDF.n_cols;i++)
            {
                stream.writeStartElement("feature");
                stream.writeAttribute("id",cnamesNum.at(i));   //CHANGE HERE (TAKE ORIGINAL) AND ADD SAMPLE INFO
                stream.writeCharacters(colToString(i));
                stream.writeEndElement();
            }
        stream.writeEndElement();
        stream.writeStartElement("samples");
        stream.writeAttribute("size",QString("[1,%1]").arg(rnames.count()));
        stream.writeCharacters(rnames.join(","));
        stream.writeEndElement();
    stream.writeEndDocument();
    return true;
}

bool DataFrame::readFromXML(QIODevice *device)
{
    if(!device)
        return false;

    bool isMetaXml = false;

    QXmlStreamReader stream(device);

    while (!stream.atEnd())
    {
        stream.readNext();

        if(stream.name() == "metaxml" && stream.isStartElement())
        {
            isMetaXml = true;

            while(!stream.isEndElement() || stream.name() != "metaxml")
            {
                stream.readNext();
                if(stream.name()=="metadata")
                {
                    stream.readNext();
                    while(!stream.isEndElement() || stream.name() != "metadata")
                    {
                        if(stream.name()=="section" && stream.isStartElement())
                        {
                            if(stream.attributes().value("purity")=="class")
                            {
                                className = stream.attributes().value("id").toString();
                                classCol = stream.readElementText().split(",");
                            }
                            else
                            {
                                namesMetaData.append(stream.attributes().value("id").toString());
                                metaDF.append(stream.readElementText().split(","));
                            }
                        }
                        stream.readNext();
                    }
                }else if(stream.name()=="data")
                {
                    QString size = stream.attributes().value("size").toString();
                    numDF.set_size(size.split(",")[0].remove(QChar('[')).toDouble(),size.split(",")[1].remove(QChar(']')).toDouble());
                    int feat=0;
                    stream.readNext();

                    while(!stream.isEndElement() || stream.name() != "data")
                    {
                        if(stream.name()=="feature")
                        {
                            cnamesNum.append(stream.attributes().value("id").toString());
                            colFromString(stream.readElementText(),feat);
                            feat++;
                        }
                        stream.readNext();
                    }
                }else if(stream.name()=="samples")
                    rnames = stream.readElementText().split(",");
            }
            cnames.append(className);
            if(!namesMetaData.isEmpty())
                cnames.append(namesMetaData);
            cnames.append(cnamesNum);
        }
    }
    if(!isMetaXml)
        qDebug() << "File does not contain metaxml info";

    if(stream.hasError())
        return false;
    return true;
}

void DataFrame::exportTo(QFile* file)
{
    QString str = file->fileName().section('.',-1);

    QChar sep;

    str=="csv"?sep=',':sep='\t';

    QTextStream out(file);

    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(5);

    out.setNumberFlags(QTextStream::ForcePoint);

    out << sep << className << sep;

    for(int j=0;j<namesMetaData.length();j++)
        out << namesMetaData.at(j) << sep;

    out << cnamesNum.join(sep) << '\n';

    for(int i=0;i<rnames.length();i++)
    {
        out << rnames.at(i) << sep;

        out << classCol.at(i) << sep;

        for(int j=0;j<namesMetaData.length();j++)
            out << metaDF.at(i).at(j) << sep;

        for(unsigned int j=0;j<numDF.n_cols;j++)
            out << numDF(i,j) << sep;

        out << '\n';
    }
}
