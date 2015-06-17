
#ifndef _PLANARRAD_DATALISTMODEL_H
#define _PLANARRAD_DATALISTMODEL_H

#include <QAbstractListModel>
#include "dataset.h"
#include "base/jlist.h"

using namespace jude::base;



class DataListModel : public QAbstractListModel {

  JList<DataSet> data_list;

  double max_depth;

  double min_waveln;
  double max_waveln;

  void recalcStats();

public:

  DataListModel();

  int dataSetCount() const { return data_list.count(); }

  double maxDepth() const { return max_depth; }

  double minWavelength() const { return min_waveln; }
  double maxWavelength() const { return max_waveln; }

  void addDataSet(DataSet& ds);
  void removeDataSet(int r);

  const DataSet* dataSet(int i) const { return (i < data_list.count()) ? data_list.at(i) : 0; }

  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;

  int rowCount(const QModelIndex& = QModelIndex()) const { return data_list.count();  }
  int columnCount(const QModelIndex& = QModelIndex()) const { return 14; }
};



#endif // _PLANARRAD_DATALISTMODEL_H

