#MetaProcess

This software allows data treatment, univariate and multivariate analysis.

It expects data input to be fed as .csv, .txt or .tab.

It has been designed for metabolomics mainly, however it can be adapted to different fields requiring statistical analysis and data transformation.

##Usage

To start a project from scratch, simply prepare a .csv, .txt or .tab file containing a table with your variables and samples. It does not matter whether rows contain samples and columns contain variables or viceversa. When you upload your file you MetaProcess can transpose your data. However, it is important that when you are set up your project your variables must be in columns.

It is convenient also to add some sample names and variable names. A row (or a column) must contain metadata (in the future it will support multiple rows or columns of metadata). You need to tell MetaProcess where you store your metadata. MetaProcess needs to know this in order to perform missing value imputation, transformation and statistics.

##Building

MetaProcess relies on [Qt](https://www.qt.io/) and [Armadillo](http://arma.sourceforge.net/). Once you have installed the appropriate libraries you can build the project as follows.

Enter the directory of the project in command line. `export` the `CMAKE_PREFIX_PATH` to your Qt location. In my case it was
```
export CMAKE_PREFIX_PATH=~/Qt/5.4/clang_64
```
and then type
```
mkdir build 
cd build 
cmake ..
make 
```