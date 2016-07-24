#ifndef PROCESSDF_H
#define PROCESSDF_H

#include <dataframe.h>

class ProcessDF : public DataFrame
{
    Q_OBJECT
public:
    enum NormMode {TIC,PQN};
    enum MVIMode {SmallValue,Mean,Median,KNN,BPCA};
    enum TransformMode {ihs,nlog,glog};
    enum ScalingMode {pareto,range,autoscal};

    ProcessDF(QObject* parent=0);

    int NoiseFill(int percent,const QString& meta,QString& msg);
    int PeakFilter(int percentFeat,int percentSamp,bool sample,QString& msg);
    int QAssurance(const QString& QCLabel, int rsdPercent,QString& msg); //returns the featurenames excluded
    int Normalisation(ProcessDF::NormMode mode,const QString& QCLabel,QString& msg);
    int MVImpute(ProcessDF::MVIMode mode,QString& msg);
    int Transform(ProcessDF::TransformMode mode,const QString& QCLabel,QString& msg); //returns error code for lambda
    int Scale(ProcessDF::ScalingMode mode,QString& msg);



signals:

public slots:
};

#endif // PROCESSDF_H
