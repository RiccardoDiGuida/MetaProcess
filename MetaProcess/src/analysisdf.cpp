#include "analysisdf.h"
#include "t_funcs.h"
#include "wilcox_funcs.h"
#include "wilcox_func_2.h"
#include <cmath>
#include <algorithm>
#include <QDebug>

const double EPSIL = 1E-5;
double minqval = 5000;
double ncomp = 2;        //COMPONENTS USED FOR NIPALS
double tol = 1e-09;
uint max_iter = 500;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

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

void rank(vec& v,vec& rk)
{
    uvec id = sort_index(v);
    vec tmp = v.rows(id);
    uvec rkint = sort_index(id);
    rk = conv_to<vec>::from(rkint);
    double prev = -99;
    int ct=0,low_bnd,up_bnd;
    double repl;
    for(uint i=0;i<tmp.n_elem;i++)
    {
        if(fabs(1-(tmp(i)/prev))<EPSIL)
            ct++;
        else if(ct>0)
        {
            ct++;
            low_bnd=rkint(id(i-ct))-1;
            up_bnd=rkint(id(i-1))+1;
            repl = double(low_bnd+up_bnd)/ct;
            while(ct>0)
                rk(id(i-(ct--)))=repl;
            prev=tmp(i);
        }
        else
            prev=tmp(i);
    }
    rk = rk+1;
}

AnalysisDF::AnalysisDF(QObject* parent)
    :ProcessDF(parent)
{

}

int AnalysisDF::ttest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,bool paired,bool multcomp,QString& msg,const QString& fac_pair)
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

            if(fabs(1-(var1/var2))<EPSIL)
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
    if(multcomp)
    {
        vec pvals(res.size());
        for(uint i=0;i<res.size();i++)
            pvals(i)=res[i].p_val;
        uvec pv_fin_idx = find_finite(pvals);
        vec pvals_fin = pvals.elem(pv_fin_idx);
        vec newps = Benjamini_Hoch(pvals_fin);
        int lnct=0;
        for(uword i : pv_fin_idx)
            res[i].p_val=newps(lnct++);
    }
    ct=0;
    for(uint i=0;i<res.size();i++)
        if(res[i].p_val<=0.025)
            ct++;
    msg = QString("The t-test produced %1 significant peaks").arg(ct);
    emit computationUniStatsDone();
    return 0;
}

int AnalysisDF::mannwtest(QList<AnalysisResult>& res,const QString& facName,const QString& fac1,const QString& fac2,bool paired,bool multcomp,QString& msg,const QString& fac_pair)
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
            double med1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return median(w);});
            double med2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return median(w);});
            vec x = m_1.col(i)-m_2.col(i);
            bool zeroes = any(x==0);
            x = x.elem(find_finite(x));
            x = nonzeros(x);
            int n = x.n_elem;
            bool exact = n<50;
            vec r(x.n_elem);
            rank(x,r);
            double Stat = sum(r.elem(find(x>0)));
            vec ur = unique(r);
            bool ties = (r.n_elem != ur.n_elem);
            if(exact && !ties && !zeroes)
            {
                double p;
                if(Stat>n*(n+1)/4)
                    p=psignrank(Stat-1,n,0,0);
                else
                    p=psignrank(Stat,n,1,0);
                temp.p_val = std::min(2*p,1.);
            }
            else
            {
                vec sort_r = sort(r);
                vec nties(sort_r.size());
                double prev = sort_r(0);
                int ct=1,pos=0;
                for(uint i=1;i<sort_r.n_elem;i++)
                    if(fabs(1-(sort_r(i)/prev))<EPSIL)
                        ct++;
                    else
                    {
                        prev=sort_r(i);
                        nties(pos++)=ct;
                        ct=1;
                    }
                double z = Stat-n*(n+1)/4;
                double sigma = sqrt(n*(n+1)*(2*n+1)/24-sum(pow(nties,3)-nties)/48);
                double corr=sgn(z)*0.5;
                z=(z-corr)/sigma;
                temp.p_val=std::min(pnorm5(z,0,1,1,0),pnorm5(z,0,1,0,0));
            }
            temp.FC=med1/med2;
            res.append(temp);
        }
    }
    else
    {
        for(uint i=0;i<numDF.n_cols;i++)
        {
            AnalysisResult temp;
            double med1 = FuncOnColvec(m_1.col(i),[](vec w)->double{return median(w);});
            double med2 = FuncOnColvec(m_2.col(i),[](vec w)->double{return median(w);});
            vec x = m_1.col(i);
            vec y = m_2.col(i);
            x = x.elem(find_finite(x));
            y = y.elem(find_finite(y));
            vec tot = join_cols(x,y);
            vec r(tot.n_elem);
            rank(tot,r);
            uint n_x = x.size();
            uint n_y = y.size();
            bool exact = (n_x<50)&&(n_y<50);
            uvec x_pos_in_r = linspace<uvec>(0,x.n_elem-1,x.n_elem);
            vec newv=r.rows(x_pos_in_r);
            double Stat = sum(newv)-n_x*(n_x+1)/2;
            vec ur = unique(r);
            bool ties = (r.n_elem != ur.n_elem);
            if(exact && !ties)
            {
                double p;
                if(Stat>(n_x*n_y/2))
                    p=pwilcox(Stat-1,n_x,n_y,0,0);
                else
                    p=pwilcox(Stat-1,n_x,n_y,1,0);
                temp.p_val=std::min(2*p,1.);
            }
            else
            {
                vec sort_r = sort(r);
                vec nties(ur.size());
                double prev = sort_r(0);
                int ct=1,pos=0;
                for(uint i=1;i<sort_r.n_elem;i++)
                    if(fabs(1-(sort_r(i)/prev))<EPSIL)
                        ct++;
                    else
                    {
                        prev=sort_r(i);
                        nties(pos++)=ct;
                        ct=1;
                    }
                nties(pos)=ct;
                double z = Stat-n_x*n_y/2;
                double sigma = sqrt((n_x*n_y/12)*((n_x+n_y+1)-sum(pow(nties,3)-nties)/((n_x+n_y)*(n_x+n_y-1))));
                double corr=sgn(z)*0.5;
                z=(z-corr)/sigma;
                temp.p_val=std::min(pnorm5(z,0,1,1,0),pnorm5(z,0,1,0,0));
            }
            temp.FC=med1/med2;
            res.append(temp);
        }
    }
    if(multcomp)
    {
        vec pvals(res.size());
        for(uint i=0;i<res.size();i++)
            pvals(i)=res[i].p_val;
        uvec pv_fin_idx = find_finite(pvals);
        vec pvals_fin = pvals.elem(pv_fin_idx);
        vec newps = Benjamini_Hoch(pvals_fin);
        int lnct=0;
        for(uword i : pv_fin_idx)
            res[i].p_val=newps(lnct++);
    }
    ct=0;
    for(uint i=0;i<res.size();i++)
        if(res[i].p_val<=0.025)
            ct++;
    msg = QString("Mann Whitney-U test produced %1 significant peaks").arg(ct);
    emit computationUniStatsDone();
    return 0;
}

int AnalysisDF::pca(MultitestResult& res,const QString& facName,bool alls,bool twow,const QString& fac1,const QString& fac2,
                     bool threew,const QString& fac11,const QString& fac12,const QString& fac13,QString& msg)
{
    rowvec MeanCol(numDF.n_cols);
    mat U;
    mat cpDF;
    mat numDF2;
    QStringList FactorsClass;

    if(facName==className)
        FactorsClass = classCol;
    else
    {
        int Idx = namesMetaData.indexOf(facName);
        for(int i=0;i<metaDF.count();i++)
            FactorsClass.append(metaDF.at(i).at(Idx));
    }

    res.className = facName;

    if(alls)
    {
        numDF2 = numDF;
        cpDF.set_size(numDF2.n_rows,numDF2.n_cols);
        res.idxs_samples = linspace<uvec>(0,numDF2.n_rows-1,numDF2.n_rows);
    }
    else if(twow)
    {
        uvec Idx_fac1,Idx_fac2;
        int ct=0;
        int start=0; 

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
        res.idxs_samples = join_cols(Idx_fac1,Idx_fac2);
        res.idxs_samples = sort(res.idxs_samples);
        numDF2 = numDF.rows(res.idxs_samples);
        cpDF.set_size(numDF2.n_rows,numDF2.n_cols);
    }
    else
    {
        uvec Idx_fac1,Idx_fac2,Idx_fac3;
        int ct=0;
        int start=0;

        while(FactorsClass.indexOf(fac11,start)!=-1)
        {
            Idx_fac1.insert_rows(ct,uvec{(uint)FactorsClass.indexOf(fac11,start)});
            start = *(Idx_fac1.end()-1);
            ct++;
            start++;
        }
        ct=0;
        start=0;                 //Collect indexes of factor2
        while(FactorsClass.indexOf(fac12,start)!=-1)
        {
            Idx_fac2.insert_rows(ct,uvec{(uint)FactorsClass.indexOf(fac12,start)});
            start = *(Idx_fac2.end()-1);
            ct++;
            start++;
        }
        ct=0;
        start=0;                 //Collect indexes of factor3
        while(FactorsClass.indexOf(fac13,start)!=-1)
        {
            Idx_fac3.insert_rows(ct,uvec{(uint)FactorsClass.indexOf(fac13,start)});
            start = *(Idx_fac3.end()-1);
            ct++;
            start++;
        }
        res.idxs_samples = join_cols(Idx_fac1,Idx_fac2);
        res.idxs_samples = join_cols(res.idxs_samples,Idx_fac3);
        res.idxs_samples = sort(res.idxs_samples);
        numDF2 = numDF.rows(res.idxs_samples);
        cpDF.set_size(numDF2.n_rows,numDF2.n_cols);
    }

    for(uint i=0;i<numDF.n_cols;i++)
    {
        MeanCol(i) = FuncOnColvec(numDF2.col(i),[](vec w)->double{return mean(w);});
        cpDF.col(i) = numDF2.col(i)-MeanCol(i);
    }

    if(cpDF.is_finite())        //is no NAs then do singular value decomposition
    {
        svd(U,res.latent,res.coeff,cpDF);
        res.score = cpDF * res.coeff;
        res.latent = res.latent/sqrt(cpDF.n_rows-1);
    }
    else        //NIPALS
    {
        uint nc = cpDF.n_cols;
        uint nr = cpDF.n_rows;
        mat p(nc,ncomp);
        mat t_mat(nr,ncomp);
        vec eig(ncomp);
        rowvec nc_ones(nc,fill::ones);
        rowvec nr_ones(nr,fill::ones);
        rowvec VarCol(numDF.n_cols);
        uint id;

        for(uint h=0;h<=ncomp;h++)
        {
            for(uint i=0;i<cpDF.n_cols;i++)
                VarCol(i) = FuncOnColvec(cpDF.col(i),[](vec w)->double{return var(w);});

            VarCol.max(id);
            vec th = cpDF.col(id);
            uvec nonFin = find_nonfinite(cpDF);
            uvec nonFin_t = find_nonfinite(cpDF.t());
            if(!th.is_finite())
                th.elem(find_nonfinite(th)).zeros();
            vec ph_old(nc);
            ph_old.fill(1/sqrt(nc));
            vec ph_new(nc);
            uint iter=1,diff=1;

            mat x_aux=cpDF;
            x_aux.elem(nonFin).zeros();

            while(diff>tol && iter<=max_iter)
            {
                ph_new = x_aux.t() * th;
                mat Th = th * nc_ones;
                Th.elem(nonFin).zeros();
                mat th_cross = Th.t()*Th;
                ph_new = ph_new/th_cross.diag();

                ph_new = ph_new/sqrt(dot(ph_new,ph_new));

                th = x_aux * ph_new;
                mat P = ph_new * nr_ones;
                P.elem(nonFin_t).zeros();
                mat ph_cross = P.t()*P;
                th = th/ph_cross.diag();

                vec tmp = pow(ph_new-ph_old,2);
                diff = sum(tmp.elem(find_finite(tmp)));
                ph_old = ph_new;
                iter++;
            }
            if(iter>max_iter)
                msg = QString("Maximum number of iterations reached for comp%1").arg(h);
            cpDF = cpDF - (th * ph_new.t());
            p.col(h) = ph_new;
            t_mat.col(h) = th;
            th = pow(th,2);
            eig(h)=sum(th.elem(find_finite(th)));
        }
        eig=sqrt(eig);

        res.coeff = p;
        res.score = cpDF * p;
        res.latent = eig/sqrt(cpDF.n_rows-1);        // OMITTED LAST PART OF IMPLEMENTATION ON MIXOMICS BECAUSE OF NO USE
    }
/*    numDF2.print();
    res.score.print();

    QFile file("C:/Users/Riccardo-Admin/Documents/numDF2.csv");
    if (file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&file);

        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(5);

        out.setNumberFlags(QTextStream::ForcePoint);

        for(int i=0;i<numDF2.n_rows;i++)
        {
            for(unsigned int j=0;j<numDF2.n_cols;j++)
                out << numDF2(i,j) << ",";

            out << '\n';
        }
    }
    QFile file2("C:/Users/Riccardo-Admin/Documents/score.csv");
    if (file2.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QTextStream out(&file2);

        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setRealNumberPrecision(5);

        out.setNumberFlags(QTextStream::ForcePoint);

        for(int i=0;i<res.score.n_rows;i++)
        {
            for(unsigned int j=0;j<res.score.n_cols;j++)
                out << res.score(i,j) << ",";

            out << '\n';
        }
    } */

    emit computationMultiStatsDone();
    return 0;
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
