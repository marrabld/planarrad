
// datasetcombo.h

#ifndef _PLANARRAD_DATASETCOMBO_H
#define _PLANARRAD_DATASETCOMBO_H

#include <QComboBox>
#include "datalistmodel.h"



class DataSetCombo : public QComboBox {

  Q_OBJECT

  const DataListModel& data_model;

  const DataSet* prev_ds;
  const DataSet* curr_ds;

  bool include_names;

public:

  DataSetCombo(const DataListModel& data_model, QWidget* p);

  const DataSet* previousDataSet() const { return prev_ds; }
  const DataSet* currentDataSet() const { return curr_ds; }
 
  void setIncludeNames(bool f) { include_names=f; }
 
  void rebuild();

public slots:

  void slotCurrentIndexChanged();

signals:

  void signalUserChange();
};




#endif // _PLANARRAD_DATASETCOMBO_H


