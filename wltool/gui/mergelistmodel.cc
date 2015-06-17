
#include "mergelistmodel.h"



int MergeListModel::columnCount(const QModelIndex&) const {
  if (!af) return 0;
  if (af->channelMerge().recordCount() == 0) return 0;
  const Record* rec = af->channelMerge().record(0);
  if (!rec) return 0;
  return rec->processedColumnsCount();
}



QVariant MergeListModel::data(const QModelIndex &index, int role) const {

  if (!af || role!=Qt::DisplayRole) return QVariant();

  const Record* rec = af->channelMerge().record(index.row());
  if (!rec) return QVariant();

  JString s(" "+rec->processed(index.column())+" ");

  return J2Q(s);
}



QVariant MergeListModel::headerData(int section, Qt::Orientation orientation, int role) const {

  if (!af || role!=Qt::DisplayRole) return QVariant();

  if (orientation==Qt::Vertical) return section+1;

  const Record* rec = af->channelMerge().record(0);
  if (!rec) return QVariant();

  JString s(rec->processedHeader(section));

  return J2Q(s);
}


