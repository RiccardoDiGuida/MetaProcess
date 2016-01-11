#include "mainwindow.h"
#include <armadillo>
#include <QApplication>
#include <iostream>

using namespace arma;

void chg(arma::subview_col<double> && cos) {cos(0)=99;}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
/*     vec r ={ 0.42 ,0.231,0.004 ,0.2345,0.1, 0.05,0.002, 0.1, 0.003, 0.996};
      r.print();
      std::cout << "\nAFTER\n";
      vec n = Benjamini_Hoch(r);
      n.print();
}*/
//    vec r(A.n_rows);
//    FuncOnMat(A,r,[](rowvec const& w){return mean(w);});
//    std::cout << r;

 //   std::cout << r%A.col(0);
