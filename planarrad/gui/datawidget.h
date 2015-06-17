
// datawidget.h

#ifndef _PLANARRAD_DATAWIDGET_H
#define _PLANARRAD_DATAWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QWidget>
#include "datalistmodel.h"

//using namespace jude::fe;



class DataWidget : public QWidget {

  Q_OBJECT

  DataListModel& data_model;

  QTableView* table_view;

  QPushButton* load_pb;
  QPushButton* clear_pb;
  QPushButton* remove_pb;

  QString current_dir_qs;

  double max_depth;

public:

  DataWidget(DataListModel& data_model, QWidget* parent);

  void setCurrentDir(const QString& qs) { current_dir_qs = qs; }

  void autoLoad(const JString& fp);

  double maxDepth() const { return max_depth; }

public slots:

  void slotLoad();
  void slotRemove();
  void slotUnselect();
  void slotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

signals:

  void signalDataSetsChanged();
  void signalReportError();
};



#endif // _PLANARRAD_DATAWIDGET_H

