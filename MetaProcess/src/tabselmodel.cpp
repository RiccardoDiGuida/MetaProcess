#include "tabselmodel.h"
#include <QDebug>

TabSelModel::TabSelModel(QAbstractItemModel * model)
    : QItemSelectionModel(model)
{
  //  connect(this,SIGNAL(currentChanged(const QModelIndex&)),SLOT(CheckSelection(const QModelIndex&)));
}

void TabSelModel::CheckSelection(const QModelIndex& mod)
{
    qDebug() << "Inside mod.column=" << mod.column();
    QModelIndexList ls = selectedColumns();

    foreach(QModelIndex i, ls)
        qDebug() << i.column() << "," << i.row();
  /*  if(selectedColumns().isEmpty());
    else
    qDebug() << "Col Selected:" << selectedColumns()[0].column(); */

    if(isColumnSelected(mod.column(),mod.parent())){
        ColSelected = qMakePair(true,mod);qDebug() << "Reaches!";}
    else
        ColSelected.first = false;
}
