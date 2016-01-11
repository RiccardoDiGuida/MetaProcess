#ifndef PRINTTWOSAMPLETEST_H
#define PRINTTWOSAMPLETEST_H

#include "analysisresult.h"
#include "analysisdf.h"

class PrintTwoSampleTest
{
public:
    PrintTwoSampleTest(const QList<AnalysisResult>& lst,const AnalysisDF* df, QFile* file);

signals:

public slots:
};

#endif // PRINTTWOSAMPLETEST_H
