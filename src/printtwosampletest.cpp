#include "printtwosampletest.h"
#include <QTextStream>

PrintTwoSampleTest::PrintTwoSampleTest(const QList<AnalysisResult>& lst,const AnalysisDF* df, QFile* file)
{
    QString str = file->fileName().section('.',-1);

    QChar sep;

    str=="csv"?sep=',':sep='\t';

    QTextStream out(file);

    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(5);

    out.setNumberFlags(QTextStream::ForcePoint);

    out << "IDX" << sep << "p-value" << sep << "FC" << sep << "CI" << "\n";

    for(uint i=0;i<df->numDF.n_cols;i++)
        out << df->cnamesNum[i] << sep << lst[i].p_val << sep << lst[i].FC << sep << lst[i].conf_int.first << "/" << lst[i].conf_int.second << "\n";
}

