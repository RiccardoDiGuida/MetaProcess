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

