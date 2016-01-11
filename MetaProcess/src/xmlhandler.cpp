#include "xmlhandler.h"

XMLHandler::XMLHandler(DataFrame &DF,QObject* parent)
    :QObject(parent)
{
    xmlDF = &DF;
}

XMLHandler::XMLHandler(QObject* parent)
    :QObject(parent)
{
    xmlDF = new DataFrame();
}

bool XMLHandler::write(QIODevice* device)
{
    QXmlStreamWriter stream(device);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("metaxml");
    stream.writeAttribute("version","0.1");
        stream.writeStartElement("metadata");
            stream.writeStartElement("section");
            stream.writeAttribute("type","string");
            stream.writeAttribute("size",QString("[%1,1]").arg(xmlDF->classCol->count()));
            stream.writeAttribute("id",*(xmlDF->className));
            stream.writeCharacters(xmlDF->classCol->join(","));
            stream.writeEndElement();
            for(int i=0;i<xmlDF->metaDF[0].count();i++)
            {
                stream.writeStartElement("section");
                stream.writeAttribute("type","string");
                stream.writeAttribute("size",QString("[%1,1]").arg(xmlDF->classCol->count()));
                stream.writeAttribute("id",xmlDF->namesMetaData->at(i));
                stream.writeCharacters(xmlDF->classCol->join(","));
                stream.writeEndElement();
            }
        stream.writeEndElement();
        stream.writeStartElement("data");
        stream.writeAttribute("size",QString("[%1,%2]").arg(xmlDF->numDF.n_rows).arg(xmlDF->numDF.n_cols));
            for(int i=0;i<xmlDF->numDF.n_cols;i++)
            {
                stream.writeStartElement("feature");
                stream.writeAttribute("id",QString("Peak_%1").arg(i+1));   //CHANGE HERE (TAKE ORIGINAL) AND ADD SAMPLE INFO
                stream.writeCharacters(xmlDF->colToString(i));
                stream.writeEndElement();
            }
        stream.writeEndElement();
    stream.writeEndDocument();
}
