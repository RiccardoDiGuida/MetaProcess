#include "mainwindow.h"
#include <armadillo>
#include <QApplication>
#include <iostream>

using namespace arma;

void chg(arma::subview_col<double> && cos) {cos(0)=99;}


int main(int argc, char *argv[])
{
    mat A = randu<mat>(5,4);

    mat coeff;
    mat score;
    vec latent;
    vec tsquared;

    princomp(coeff, score, latent, tsquared, A);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

//    vec r(A.n_rows);
//    FuncOnMat(A,r,[](rowvec const& w){return mean(w);});
//    std::cout << r;

 //   std::cout << r%A.col(0);
