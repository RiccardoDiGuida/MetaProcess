#include "analysisdf.h"
#include "t_funcs.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

const double EPSIL = 1E-5;
double minqval = 5000;

template<typename Func>
double FuncOnColvec(vec&& v,Func const & func)
{
    if(v.is_finite())
        return func(v);
    else if(!any(v>0))
        return NAN;
    else
        return func(v.elem(find_finite(v)));
}

template <typename T>
QList<uint> sort_indexes(const QList<T> &v)
{
  // initialize original index locations
  QList<uint> idx;
  idx.reserve(v.size());
  for(uint i=0; i!=idx.size(); ++i) idx[i] = i;

  // sort indexes based on comparing values in v
  std::sort(idx.begin(), idx.end(),
       [&v](uint i1, uint i2) {return v[i1] < v[i2];});

  return idx;
}

vec Benjamini_Hoch(vec p)
{
    uint n = p.size();   //for reference go to http://stackoverflow.com/questions/10323817/r-unexpected-results-from-p-adjust-fdr
    uvec id = sort_index(p);
    vec tmp(n),q(n);
    tmp=p.rows(id);
    for(uint i=0;i<n;i++)
        q(i) = tmp(i)*n/(i+1);
    for(int i=n-1;i>=0;i--)
    {
        if(q(i)>minqval)
            q(i)=minqval;
        else
            minqval=q(i);
    }
    uvec id_fin = sort_index(id);
    return q.rows(id_fin);
}

AnalysisDF::AnalysisDF(QObject* parent)
    :ProcessDF(parent)
{

}

int AnalysisDF::ttest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,bool paired,QString& msg,const QString& fac_pair)
{
    uvec Idx_fac1,Idx_fac2;
    QStringList FactorsClass;
    int ct=0;
    int start=0;                 //Collect indexes of factor1

    if(fac1==fac2)
    {
        msg = "Factors must be different!";
        return 1;
    }

    if(facName==className)
        FactorsClass = classCol;
    else
    {
        int Idx = namesMetaData.indexOf(facName);
        for(int i=0;i<metaDF.count();i++)
            FactorsClass.append(metaDF.at(i).at(Idx));
    }

    while(FactorsClass.indexOf(fac1,start)!=-1)
    {
        Idx_fac1.insert_rows(ct,uvec{(uint)FactorsClass.indexOf(fac1,start)});
        start = *(Idx_fac1.end()-1);
        ct++;
        start++;
    }
    ct=0;
    start=0;                 //Collect indexes of factor2
    while(FactorsClass.indexOf(fac2,start)!=-1)
    {
        Idx_fac2.insert_rows(ct,uvec{(uint)FactorsClass.indexOf(fac2,start)});
        start = *(Idx_fac2.end()-1);
        ct++;
        start++;
    }

    bool eq_size=false;
    double n1 = Idx_fac1.n_elem; //sample size 1
    double n2 = Idx_fac2.n_elem;
    if(n1==n2)
        eq_size = true;

    mat m_1 = numDF.rows(Idx_fac1);
    mat m_2 = numDF.rows(Idx_fac2);

    if(paired)
    {
        if(!eq_size)
        {
            msg = "Sample sizes must be equal for paired test!";
            return 1;
        }
        else if(fac_pair==facName)
        {
            msg= "Pairing factor must be different from separating factor!";
            return 1;
        }

        QStringList Pairfacs,pairfac1,pairfac2;
        uvec idx_pairfac1(n1),idx_pairfac2(n1);

        if(fac_pair==className)
            Pairfacs = classCol;
        else
        {
            int Idx = namesMetaData.indexOf(fac_pair);
            for(int i=0;i<metaDF.count();i++)
                Pairfacs.append(metaDF.at(i).at(Idx));
        }

        for(uint i=0;i<n1;i++)
        {
            pairfac1.append(Pairfacs[Idx_fac1(i)]);
            pairfac2.append(Pairfacs[Idx_fac2(i)]);
        }

        uint j=0;
        QList<uint> idx_fac1_l,idx_fac2_l;
        idx_fac1_l = sort_indexes(pairfac1);
        for (auto i: idx_fac1_l)
          idx_pairfac1(j++) = i;

        j=0;
        idx_fac2_l = sort_indexes(pairfac2);
        for (auto i: idx_fac2_l)
          idx_pairfac2(j++) = i;

        for(uint i=0;i<n1;i++)
            if(pairfac1[idx_pairfac1(i)]!=pairfac2[idx_pairfac2(i)])
            {
                msg = "Every pairing factor must have one related factor in the other class";
                return 1;
            }

        uvec cols = linspace<uvec>(0,m_1.n_cols-1,m_1.n_cols);
        m_1 = m_1.submat(idx_pairfac1,cols);
        m_2 = m_2.submat(idx_pairfac2,cols);

        for(uint i=0;i<numDF.n_cols;i++)
        {
            AnalysisResult temp;
            Beta beta;
            double cov=0.0;
            double var1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return var(w);});
            double var2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return var(w);});

            double m1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return mean(w);});
            double m2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return mean(w);});

            if(std::isnan(var1)||std::isnan(var2))
            {
                res.append(temp);
                continue;
            }

            double med1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return median(w);});
            double med2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return median(w);});

            for (uint j=0;j<n1;j++) cov += (m_1(i,j)-m1)*(m_2(i,j)-m2);
            cov /= (temp.dfreedom=n1-1);
            double sd = sqrt((var1+var2-2.0*cov)/n1);
            temp.t_val = (m1-m2)/sd;
            temp.p_val=beta.betai(0.5*temp.dfreedom,0.5,temp.dfreedom/(temp.dfreedom+temp.t_val*temp.t_val));

            temp.FC=med1/med2;
            res.append(temp);
        }

    }else{

        for(uint i=0;i<numDF.n_cols;i++)
        {
            AnalysisResult temp;
            double var1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return var(w);});
            double var2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return var(w);});

            double m1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return mean(w);});
            double m2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return mean(w);});

            if(std::isnan(var1)||std::isnan(var2))
            {
                res.append(temp);
                continue;
            }

            double med1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return median(w);});
            double med2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return median(w);});

            if(fabs(var1-var2)<EPSIL)
            {
                Beta beta;
                temp.dfreedom = n1+n2-2;
                double svar=((n1-1)*var1+(n2-1)*var2)/temp.dfreedom;
                temp.t_val=(m1-m2)/sqrt(svar*(1.0/n1+1.0/n2));
                temp.p_val=beta.betai(0.5*temp.dfreedom,0.5,temp.dfreedom/(temp.dfreedom+temp.t_val*temp.t_val));
            }else
            {
                Beta beta;
                temp.t_val = fabs(m1-m2)/sqrt(var1/n1+var2/n2);
                temp.dfreedom = pow(var1/n1+var2/n2,2)/(pow(var1/n1,2)/(n1-1)+pow(var2/n2,2)/(n2-1));
                temp.p_val=beta.betai(0.5*temp.dfreedom,0.5,temp.dfreedom/(temp.dfreedom+temp.t_val*temp.t_val));
            }
            temp.FC=med1/med2;
            res.append(temp);
        }
    }
    ct=0;
    for(uint i=0;i<res.size();i++)
        if(res[i].p_val<=0.025)
            ct++;
    msg = QString("The t-test produced %1 significant peaks").arg(ct);
    emit computationUniStatsDone();
    return 0;
}

void AnalysisDF::mannwtest()
{

}

void AnalysisDF::pca()
{

}

void AnalysisDF::PrintTwoSampleTest(const QList<AnalysisResult>& lst, QFile* file)
{
    QString str = file->fileName().section('.',-1);

    QChar sep;

    str=="csv"?sep=',':sep='\t';

    QTextStream out(file);

    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(5);

    out.setNumberFlags(QTextStream::ForcePoint);

    out << "IDX" << sep << "p-value" << sep << "FC" << sep << "CI" << "\n";

    for(uint i=0;i<numDF.n_cols;i++)
        out << cnamesNum[i] << sep << lst[i].p_val << sep << lst[i].FC << sep << lst[i].conf_int.first << "/" << lst[i].conf_int.second << "\n";
}
