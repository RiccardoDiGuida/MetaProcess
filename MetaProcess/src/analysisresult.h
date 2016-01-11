#ifndef ANALYSISRESULT
#define ANALYSISRESULT

#include <QPair>

class AnalysisResult
{
public:
    AnalysisResult():
        t_val(NAN),p_val(NAN),dfreedom(NAN),FC(NAN),conf_int(NAN,NAN){}
    double t_val;
    double p_val;
    double dfreedom;
    double FC;
    QPair<double,double> conf_int;  //confidence interval
};

#endif // ANALYSISRESULT

