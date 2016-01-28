#ifndef MULTITESTRESULT
#define MULTITESTRESULT

#include <armadillo>

using namespace arma;

class MultitestResult
{
public:
    mat coeff;
    mat score;
    vec latent;
    vec tsquared;
};

#endif // MULTITESTRESULT

