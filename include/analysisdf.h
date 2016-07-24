#ifndef ANALYSISDF_H
#define ANALYSISDF_H

#include "processdf.h"
#include "analysisresult.h"
#include "multitestresult.h"


class AnalysisDF : public ProcessDF
{
    Q_OBJECT
public:
    AnalysisDF(QObject* parent=0);
    int ttest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,
               bool paired,bool multcomp,QString& msg,const QString& fac_pair="");
    int mannwtest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,
                   bool paired,bool multcomp,QString& msg,const QString& fac_pair="");
    int aov(QList<AnalysisResult>& res,const QString& facName,bool multcomp,bool tukey,QString& msg);
    int pca(MultitestResult& res,const QString& facName,bool alls,bool twow,const QString& fac1,const QString& fac2,
             bool threew,const QString& fac11,const QString& fac12,const QString& fac13,QString& msg);

    void PrintTwoSampleTest(const QList<AnalysisResult>& lst, QFile* file);

signals:
    void computationUniStatsDone();
    void computationMultiStatsDone();
};

#endif // ANALYSISDF_H
