#ifndef MULTITESTRESULT
#define MULTITESTRESULT

#include <armadillo>
#include <QString>

using namespace arma;

class MultitestResult
{
public:
    mat coeff;
    mat score;
    vec latent;
    uvec idxs_samples;
    QString className;
};

#endif // MULTITESTRESULT

