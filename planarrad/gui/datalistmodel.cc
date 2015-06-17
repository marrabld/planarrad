
#include "datalistmodel.h"



DataListModel::DataListModel() : 
  max_depth(0),
  min_waveln(500),
  max_waveln(500) { 

  data_list.setAutoDelete(true);
}



void DataListModel::addDataSet(DataSet& ds) { 

  int r = rowCount();
  beginInsertRows(QModelIndex(), r, r);
  data_list.append(&ds); 
  endInsertRows();

  if (ds.maxDepth() > max_depth || r==0) max_depth = ds.maxDepth();

  if (ds.minWavelength() < min_waveln || r==0) min_waveln = ds.minWavelength();
  if (ds.maxWavelength() < max_waveln || r==0) max_waveln = ds.maxWavelength();
}



void DataListModel::removeDataSet(int r) { 

  if (r < 0 || r >= data_list.count()) return;

  beginRemoveRows(QModelIndex(), r, r);
  data_list.at(r);
  data_list.removeAtCurrentPos();
  endRemoveRows();

  recalcStats();
}



// DataListModel::recalcStats() [private]

void DataListModel::recalcStats() {

  max_depth = 0;
  DataSet* ds = data_list.first();
  while (ds) {
    if (ds->maxDepth() > max_depth) max_depth = ds->maxDepth();
    ds = data_list.next();
  }
}



QVariant DataListModel::data(const QModelIndex &index, int role) const {

  if (role==Qt::TextAlignmentRole) {
    return QVariant((index.column() != 0 && index.column() != 13) ? Qt::AlignCenter : (Qt::AlignLeft | Qt::AlignVCenter));
  }

  if (role!=Qt::DisplayRole) return QVariant();

  DataSet* ds = data_list.at(index.row());
  if (!ds) return "";

  switch (index.column()) {
    case 0 : return QVariant(QString(" ")+J2Q(ds->name())+" ");
    case 1 : return QVariant(ds->isModel() ? " MODEL " : " DATA ");
    case 2 : return (ds->EdSet().hasSampleData() ? "Y" : " ");
    case 3 : return (ds->EuSet().hasSampleData() ? "Y" : " ");
    case 4 : return (ds->LdSet().hasSampleData() ? "Y" : " ");
    case 5 : return (ds->LuSet().hasSampleData() ? "Y" : " ");
    case 6 : return (ds->EoSet().hasSampleData() ? "Y" : " ");
    case 7 : return (ds->EodSet().hasSampleData() ? "Y" : " ");
    case 8 : return (ds->EouSet().hasSampleData() ? "Y" : " ");
    case 9 : return (ds->KdSet().hasSampleData() ? "Y" : " ");
    case 10 : return (ds->KuSet().hasSampleData() ? "Y" : " ");
    case 11 : return (ds->RSet().hasSampleData() ? "Y" : " ");
    case 12 : return (ds->RtotSet().hasSampleData() ? "Y" : " ");
    case 13 : return QVariant(J2Q(ds->filePath()));
  }

  return QVariant();
}



QVariant DataListModel::headerData(int section, Qt::Orientation orientation, int role) const {

  //if (role==Qt::SizeHintRole && section==0) return QVariant(Qt::MaximumSize);

  if (role!=Qt::DisplayRole) return QVariant();

  if (orientation==Qt::Vertical) return section+1;

  switch (section) {
    case 0 : return "Name";
    case 1 : return "Type";
    case 2 : return "Ed";
    case 3 : return "Eu";
    case 4 : return "Ld";
    case 5 : return "Lu";
    case 6 : return "Eo";
    case 7 : return "Eod";
    case 8 : return "Eou";
    case 9 : return "Kd";
    case 10 : return "Ku";
    case 11 : return "R";
    case 12 : return "Rtot";
    case 13 : return "File path";
  }

  //const Record* rec = af->channelMerge().record(0);
  //if (!rec) return QVariant();

  return QVariant();
}


