#ifndef ANALYSISDF_H
#define ANALYSISDF_H

#include "processdf.h"
#include "analysisresult.h"


class AnalysisDF : public ProcessDF
{
    Q_OBJECT
public:
    AnalysisDF(QObject* parent=0);
    int ttest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,
               bool paired,bool multcomp,QString& msg,const QString& fac_pair="");
    int mannwtest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,
                   bool paired,bool multcomp,QString& msg,const QString& fac_pair="");
    void pca();

    void PrintTwoSampleTest(const QList<AnalysisResult>& lst, QFile* file);

signals:
    void computationUniStatsDone();
};

#endif // ANALYSISDF_H
