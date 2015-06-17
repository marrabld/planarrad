
#include "recordlistmodel.h"



QVariant RecordListModel::data(const QModelIndex &index, int role) const {

  if (role == Qt::TextAlignmentRole) {
    if (index.column() >= 2) return QVariant(Qt::AlignCenter | Qt::AlignVCenter);
    if (index.column() == 0) return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
  }

  if (!af || role!=Qt::DisplayRole) return QVariant();

  Record* rec = af->record(index.row());

  if (!rec) return QVariant();

  if (index.column() == 0) return QString(" ")+QString::number(rec->timeSinceStartMsec())+" ";
  //if (index.column() == 1) return QString(" ")+rec->sensor()->typeText()+" ";
  if (index.column() == 1) {
    JString s = rec->typeText();
    return QString(" ")+J2Q(s)+" ";
  }
  if (index.column() == 2) return QString(" ")+QString::number(rec->sensor()->channelIndex() + 1)+" ";
  if (index.column() == 3) return rec->hasProblem() ? " N " : " Y ";
 
  return QVariant();
}



QVariant RecordListModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (role!=Qt::DisplayRole) return QVariant();

  if (orientation==Qt::Vertical) return section+1;

  if (section==0) return " msec ";
  if (section==1) return " type ";
  if (section==2) return " chan ";
  if (section==3) return " ok ";

  return QVariant();
}


