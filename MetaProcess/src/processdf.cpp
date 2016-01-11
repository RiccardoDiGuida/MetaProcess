#include "processdf.h"
#include "brent.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <QDebug>
#include <time.h>

int count=1;

template<typename Func>
void FuncOnMatRows(const mat& M,vec& v,Func const & func)
{
    for(uint i=0;i<M.n_rows;i++)     // operation calculated on rows
    {
        if(M.row(i).is_finite())
            v(i) = func(M.row(i));
        else if(!any(M.row(i)>0))
            v(i) = NAN;
        else
        {
            rowvec b=M.row(i);
            v(i) = func(b.elem(find_finite(b)).t());    //output of .elem is always colvec, so transpose
        }
    }
}

template<typename Func>
void FuncOnMatCols(const mat& M,rowvec& v,Func const & func)
{
    for(uint i=0;i<M.n_cols;i++)     // operation calculated on cols
    {
        if(M.col(i).is_finite())
            v(i) = func(M.col(i));
        else if(!any(M.col(i)>0))
            v(i) = NAN;
        else
        {
            vec b=M.col(i);
            v(i) = func(b.elem(find_finite(b)));
        }
    }
}

double EuclidDist(vec&& v1,vec&& v2);
vec glogF(vec&& v,double ofs,double lam);
void glogF(vec& v,double lam,arma::subview_col<double> & glogVec);
void jglog(vec&& v,double ofs,double lam,arma::subview_col<double> && glogVec);
double SSE(mat& m,double ofs,double lam,mat& z);

ProcessDF::ProcessDF(QObject* parent)
    :DataFrame(parent)
{

}

int ProcessDF::NoiseFill(int percent, const QString& meta,QString& msg)
{
    double noise=numDF.elem(find_finite(numDF)).min()/2;  // maybe change to select just values >0

    double dpercent = static_cast<double>(percent)/100;
    QStringList FactorsClass;

    if(meta==className)
        FactorsClass = classCol;
    else
    {
        int Idx = namesMetaData.indexOf(meta);
        for(int i=0;i<metaDF.count();i++)
            FactorsClass.append(metaDF.at(i).at(Idx));
    }

    QStringList CpFactors(FactorsClass);

    CpFactors.removeDuplicates();  //unique set of factors

    foreach(QString fac,CpFactors)
    {
        int start=0;
        int ct=0;
        uvec subidx;    // Indexes for each factor

        while(FactorsClass.indexOf(fac,start)!= -1)
        {
            subidx.insert_rows(ct,uvec{(uint)FactorsClass.indexOf(fac,start)});
            start = *(subidx.end()-1);     //CHECKED
            start++;
            ct++;
        }

        for(unsigned int i=0;i<numDF.n_cols;i++)
        {
            uvec b = find_nonfinite(numDF.submat(subidx,uvec{i}));
            if(static_cast<double>(b.n_elem)/subidx.size()>=dpercent)        // if NA's above certain percentage
                numDF.submat(subidx,uvec{i}).fill(noise);       // replace all values (including present) for a certain factor with noise
        }
    }
}

int ProcessDF::PeakFilter(int percentFeat, int percentSamp,bool sample,QString& msg)
{
    uvec idxKeepCol,idxKeepRow;
    int ct=0;
    QStringList::iterator itColNum(cnamesNum.begin()),itCnam(cnames.begin()+1+namesMetaData.count()),itRow(rnames.begin());

    double dpercFeat = static_cast<double>(percentFeat)/100;
    double dpercSamp = static_cast<double>(percentSamp)/100;

    for(unsigned int i=0;i<numDF.n_cols;i++)  // Peak filtering
    {
        uvec b = find_nonfinite(numDF.col(i));
        if((double)b.n_elem/numDF.n_rows<dpercFeat)        // if NAs below certain percentage
        {
            idxKeepCol.insert_rows(ct++,uvec{i});       // note the index so to retain it later
            itCnam++;
            itColNum++;
        }
        else
        {
            itCnam = cnames.erase(itCnam);
            itColNum = cnamesNum.erase(itColNum);
        }
    }

    ct=0;

    if(sample)
        for(unsigned int i=0;i<numDF.n_rows;i++)  // Sample filtering
        {
            uvec b = find_nonfinite(numDF.row(i));
            if((double)b.n_elem/numDF.n_cols<dpercSamp)        // if NAs below certain percentage
            {
                idxKeepRow.insert_rows(ct++,uvec{i});       // note the index so to retain it later
                itRow++;
            }
            else
                itRow = rnames.erase(itRow);
        }

    if(sample)
        numDF = numDF.submat(idxKeepRow,idxKeepCol);
    else
        numDF = numDF.cols(idxKeepCol);
}

int ProcessDF::QAssurance(const QString& QCLabel, int rsdPercent,QString& msg)
{
    uvec IdxQcs,keepColidx;
    QStringList removed;
    double rsdPerc = rsdPercent/100.0;
    int ct=0;

    int start=0;                 //Collect indexes of QCs
    while(classCol.indexOf(QCLabel,start)!=-1)
    {
        IdxQcs.insert_rows(ct,uvec{(uint)classCol.indexOf(QCLabel,start)});
        start = *(IdxQcs.end()-1);
        ct++;
        start++;
    }

    ct = 0;
    int totMetaData = namesMetaData.count();
    QStringList::iterator it(cnamesNum.begin()),itCnam(cnames.begin()+1+totMetaData);

    for(unsigned int i=0;i<numDF.n_cols;i++)       //Subset matrix and check RSD
    {
        vec b = numDF.submat(IdxQcs,uvec{i});
        b = b.elem(find_finite(b));

        if(b.n_elem>0)
        {
            if(stddev(b)/mean(b)<rsdPerc)    //if below retain
            {
                keepColidx.insert_rows(ct++,uvec{i});
                it++;
                itCnam++;
            }
            else                            //otherwise keep track of removed
            {
                removed.append(*it);
                it = cnamesNum.erase(it);
                itCnam = cnames.erase(itCnam);  // 1 is the class
            }
        }
        else
        {
            removed.append(*it);
            it = cnamesNum.erase(it);
            itCnam = cnames.erase(itCnam);  // 1 is the class
        }
    }

    numDF = numDF.cols(keepColidx);

    if(keepColidx.n_elem==0)
        return 1;   //no peaks are left
    else if(keepColidx.n_elem==1)
        return 2;   //only 1 peak left
    else
        return 0;   //more than 1 peak left
}

int ProcessDF::Normalisation(ProcessDF::NormMode mode, const QString &QCLabel,QString& msg)
{
    if(mode==ProcessDF::TIC)
    {
        vec TotSum(numDF.n_rows);

        for(int i=0;i<numDF.n_rows;i++){            //CHANGE TO FUNCMATONROWS
            if(numDF.row(i).is_finite()){
                TotSum(i) = sum(numDF.row(i));}
            else
            {
                rowvec b=numDF.row(i);
                TotSum(i) = sum(b.elem(find_finite(b)));
            }
        }
        numDF.each_col() /= TotSum;
        numDF *= 100;
    }
    else if(mode==ProcessDF::PQN)
    {
        uvec IdxQcs,nonacols;
        vec coefs(numDF.n_rows);
        int ct=0;

        int start=0;                 //Collect indexes of QCs
        while(classCol.indexOf(QCLabel,start)!=-1)
        {
            IdxQcs.insert_rows(ct,uvec{(uint)classCol.indexOf(QCLabel,start)});
            start = *(IdxQcs.end()-1);
            ct++;
            start++;
        }

        mat qcs = numDF.rows(IdxQcs);
        rowvec ref(qcs.n_cols);                    // reference vector, column-wise mean of qcs
        for(uint i=0;i<qcs.n_cols;i++)
        {
            if(qcs.col(i).is_finite())          //CHANGE TO FUNCMATONCOL
                ref(i) = mean(qcs.col(i));
            else if(!any(qcs.col(i)>0))
                ref(i) = NAN;
            else
            {
                vec b=qcs.col(i);
                ref(i) = mean(b.elem(find_finite(b)));
            }
        }

        ct=0;

        for(uint i=0;i<numDF.n_rows;i++)    //for every sample
        {
            mat joint = join_cols(numDF.row(i),ref);   //combine ref vector with every sample

       //     for(uint j=0;j<joint.n_cols;j++)
         //       if(joint.col(j).is_finite())
           //         nonacols.insert_rows(ct++,uvec{j});
            nonacols = find(all(joint>0)); // does the job of three previous lines much faster

            joint = joint.cols(nonacols);    // we just take columns contatining no na

            coefs(i) = median(joint.row(0)/joint.row(1));
        }
        numDF.each_col() /= coefs;
    }
}

int ProcessDF::MVImpute(ProcessDF::MVIMode mode,QString& msg)
{
    if(mode==ProcessDF::SmallValue)
        numDF.elem(find_nonfinite(numDF)).fill(min(min(numDF))/2);   // simply replace with noise
    else if(mode==ProcessDF::Mean)
    {
        for(unsigned int i=0;i<numDF.n_cols;i++)
        {
            uvec b = find_nonfinite(numDF.col(i));
            vec c = numDF.col(i);
            numDF.submat(b,uvec{i}).fill(mean(c.elem(find_finite(c))));
        }
    }
    else if(mode==ProcessDF::Median)
    {
        for(unsigned int i=0;i<numDF.n_cols;i++)
        {
            uvec b = find_nonfinite(numDF.col(i));
            vec c = numDF.col(i);
            numDF.submat(b,uvec{i}).fill(median(c.elem(find_finite(c))));
        }
    }
    else if(mode==ProcessDF::KNN)
    {
        const int k=2;
        int counter=0;
        uvec keepColidx;

        if(numDF.n_cols<=k)
        {
            msg = QString("Not enough peaks. They must be more than %1.").arg(k);
            return 99;  //99 is the generic error
        }
        for(uint i=0;i<numDF.n_cols;i++)
        {
            if(numDF.col(i).is_finite())
            {
                keepColidx.insert_rows(counter++,uvec{i});
                continue;
            }
            uvec b = find_nonfinite(numDF.col(i));
            vec c = numDF.col(i);
            if(b.n_elem/numDF.n_rows>0.5)
                numDF.submat(b,uvec{i}).fill(mean(c.elem(find_finite(c))));
            else
                keepColidx.insert_rows(counter++,uvec{i});
        }

        mat x = numDF.cols(keepColidx);
        mat y = x;
        QMap<double,int> dists;
        double weight_tmp;
        uvec nonacols;
        uvec nonacols_def;
        double TotalDist;

        for(uint i=0;i<x.n_cols;i++)
        {
            if(x.col(i).is_finite())
                continue;

            uvec mvals = find_nonfinite(x.col(i));
            uvec known = find_finite(x.col(i));
            nonacols.clear();
            nonacols_def.clear();

            qDebug() << "Column:" << i;

            foreach(uint r,mvals)
            {
                uvec full(known);
                full.insert_rows(full.n_elem,uvec{r});

                nonacols = find(all(x.rows(full)>0));

                if(nonacols.n_elem==nonacols_def.n_elem)
                    if(all(nonacols==nonacols_def))
                    {
                        double tmp=0;
                        for(uint j=0;j<k;j++)
                        {
                            weight_tmp = 1-dists.keys().at(j)/TotalDist;
                            tmp += weight_tmp*x(r,dists.values().at(j));
                        }
                        y(r,i) = tmp/(k-1);
                        continue;
                    }

                nonacols_def = nonacols;

                dists.clear();

                foreach(uint j,nonacols)
                    dists.insert(EuclidDist(x.submat(known,uvec{i}),x.submat(known,uvec{j})),j);

                TotalDist=0;
                for(uint j=0;j<k;j++)
                    TotalDist += dists.keys().at(j);

                double tmp=0;
                for(uint j=0;j<k;j++)
                {
                    weight_tmp = 1-dists.keys().at(j)/TotalDist;
                    tmp += weight_tmp*x(r,dists.values().at(j));
                }
                y(r,i) = tmp/(k-1);
            }
        }
        numDF.cols(keepColidx) = y;
    }
}

double EuclidDist(vec&& v1,vec&& v2)
{
    vec diff=v1-v2;

    return as_scalar(sqrt((diff.t())*diff));
}

int ProcessDF::Transform(ProcessDF::TransformMode mode,const QString& QCLabel,QString& msg)
{
    if(mode==TransformMode::ihs)
        numDF = log(numDF+sqrt(numDF%numDF+1));
    else if(mode==TransformMode::nlog)
        numDF = log(numDF);
    else if(mode==TransformMode::glog)
    {
        mat x=numDF;

        uvec IdxQcs;
        int ct=0;
        int start=0;                 //Collect indexes of QCs
        while(classCol.indexOf(QCLabel,start)!=-1)
        {
            IdxQcs.insert_rows(ct,uvec{(uint)classCol.indexOf(QCLabel,start)});
            start = *(IdxQcs.end()-1);
            ct++;
            start++;
        }
        x = x.rows(IdxQcs);
        x = x.t();

        double y0=0; //offset
        double scaling_fac=1;
        double power_fac=1;
        colvec rvar(x.n_rows);

 { /*
        FuncOnMatRows(x,rvar,[](rowvec const& w){return var(w);});
        //varianceRows(x,rvar);     //CHANGE TO FUNCMATONROWS

        double test=min(rvar);
        double test2=max(rvar)/min(rvar);

        mat x_tmp = x;
        ct=0;

        while(test2<=1e10||test<=1e5)
        {
            if(test<=1e5)
                scaling_fac *= 10;
            if(test2<=1e10)
                power_fac += 0.1;

            x_tmp = x*scaling_fac;
            x_tmp = pow(x,power_fac);//x_tmp.transform([power_fac](double val){return pow(val,power_fac);});
            ct++;
            colvec rvar_tmp(x.n_rows);
            FuncOnMatRows(x_tmp,rvar_tmp,[](rowvec const& w){return var(w);});
            //varianceRows(x_tmp,rvar_tmp);//CHANGE TO FUNCMATONROWS

            test=min(rvar_tmp);
            test2=max(rvar_tmp)/min(rvar_tmp);
        }
        x = x*scaling_fac;
        x = pow(x,power_fac);//x.transform([power_fac](double val){return pow(val,power_fac);});  */
        }
        double offset=min(min(x));
        x = x-offset;//x.transform([offset](double val){return val-offset;});

        double step_threshold=1e-16;

        FuncOnMatRows(x,rvar,[](rowvec const& w){return var(w);});

        double lambda;
        double low_limit=0;//-pow(offset,2);

        vec maj_0;

        if(min(rvar)==0)
            maj_0 = rvar.elem(find(rvar>0));
        else
            maj_0 = rvar;

        double cos=max(rvar)/min(maj_0);
        vec r(x.n_rows);
        r.fill(cos);
        vec m = max(rvar,r);
        double up_limit=max(m);

        class SSE_c : public brent::func_base
        {
        public:
            mat A;
            mat Out;
            double offset;
            virtual double operator() (double lam)
            {
                return SSE(A,offset,lam,Out);
            }
            SSE_c(mat a,double of,mat out) {A=a;offset=of;Out=out;}
        };

        mat z(x.n_rows,x.n_cols);

        SSE_c fun(x,y0,z);

        clock_t startTime = clock();
        brent::local_min(low_limit,up_limit,step_threshold,fun,lambda);
        std::cout << double( clock() - startTime ) / (double)CLOCKS_PER_SEC<< " seconds." << std::endl <<"Iterations: "<<count;
        //OPTIMIZE HERE. BRENT
        double lambda_std=5.0278e-9;
        int error=0; //no error

        if(fabs(up_limit-lambda)<=1e-5)
            error=3;  //lambda tends to infinity
        else if(fabs(low_limit-lambda)<=1e-5)
            error=4;  //lamba tends to -infinity

        x = numDF;
        x = x.t();

        if(error)
        {
            lambda = lambda_std;
            rowvec scal_fact_vec(x.n_cols);
            FuncOnMatCols(x,scal_fact_vec,[](vec const& w){return sum(w);});
            scaling_fac = mean(scal_fact_vec);  //may have problems is scal_fact_vec has missing values
            scaling_fac = 1/scaling_fac;
        }

        x = x*scaling_fac;
        x = pow(x,power_fac);
        offset=min(min(x));
        x = x-offset;

        for(uint i=0;i<x.n_cols;i++)
            numDF.row(i) = glogF(x.col(i),y0,lambda).t();
    }
}

double SSE(mat& m,double ofs,double lam,mat& z)
{
    for(uint i=0;i<m.n_cols;i++)
        jglog(m.col(i),ofs,lam,z.col(i));

    count++;

    double s=0;
    vec mean_spec(z.n_rows);
    FuncOnMatRows(z,mean_spec,[](rowvec const& w){return mean(w);});

    z.each_col() -= mean_spec;
    z = pow(z,2);
    s = sum(z.elem(find_finite(z)));

    return s;
}

void jglog(vec&& v,double ofs,double lam,arma::subview_col<double> && glogVec)
{
    v -=ofs;
    glogF(v,lam,glogVec);
    double gmn;
    v = log(sqrt(square(v)+lam));
    if(v.is_finite())
        gmn=exp(mean(v));
    else
        gmn=exp(mean(v.elem(find_finite(v))));

    glogVec = glogVec*gmn;
}

vec glogF(vec&& v,double ofs,double lam)
{
    return log((v-ofs)+sqrt(square(v-ofs)+lam));
}

void glogF(vec& v,double lam,arma::subview_col<double> & glogVec)
{
    glogVec = log((v)+sqrt(square(v)+lam));
}

int ProcessDF::Scale(ProcessDF::ScalingMode mode,QString& msg)
{
    rowvec MeanperCol(numDF.n_cols);
    FuncOnMatCols(numDF,MeanperCol,[](vec const& w){return mean(w);});

    if(mode==ScalingMode::autoscal)
    {
        rowvec SDperCol(numDF.n_cols);
        FuncOnMatCols(numDF,SDperCol,[](vec const& w){return stddev(w);});

        for(uint i=0;i<numDF.n_cols;i++)
            numDF.col(i) = (numDF.col(i)-MeanperCol(i))/SDperCol(i);

    }
    else if(mode==ScalingMode::pareto)
    {
        rowvec sqrtSDperCol(numDF.n_cols);
        FuncOnMatCols(numDF,sqrtSDperCol,[](vec const& w){return sqrt(stddev(w));});

        for(uint i=0;i<numDF.n_cols;i++)
            numDF.col(i) = (numDF.col(i)-MeanperCol(i))/sqrtSDperCol(i);
    }
    else if(mode==ScalingMode::range)
    {
        rowvec RngperCol(numDF.n_cols);
        FuncOnMatCols(numDF,RngperCol,[](vec const& w){return max(w)-min(w);});

        for(uint i=0;i<numDF.n_cols;i++)
            numDF.col(i) = (numDF.col(i)-MeanperCol(i))/RngperCol(i);
    }
}
