
// datatypecombo.h

#ifndef _PLANARRAD_DATATYPECOMBO_H
#define _PLANARRAD_DATATYPECOMBO_H

#include <QComboBox>
#include "datalistmodel.h"



class DataTypeCombo : public QComboBox {

  Q_OBJECT

  const DataListModel& data_model;

  const DataSet* curr_ds;

  const SpecSet* prev_ss;
  const SpecSet* curr_ss;
 
  enum { IRRAD=1, RAD=2, MM1=4, SRM1=8, PROP=16 };

public:

  DataTypeCombo(const DataListModel& data_model, QWidget* p);

  const SpecSet* previousSpecSet() const { return prev_ss; }
  const SpecSet* currentSpecSet() const { return curr_ss; }

  bool specSetChanged() const { return prev_ss != curr_ss; }

  void rebuild(const DataSet* ds);

  const SpecSet* getCurrentSpecSet() const;

  int unitsCode() const;
  static const JString unitsString(int code);

public slots:

  void slotCurrentIndexChanged();

signals:

  void signalUserChange();
};




#endif // _PLANARRAD_DATATYPECOMBO_H


