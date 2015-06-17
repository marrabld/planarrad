
#ifndef _WLTOOL_GUI_RECORDTAB_H
#define _WLTOOL_GUI_RECORDTAB_H

#include <QWidget>

class RecordListModel;
class QTableView;



// RecordTab

class RecordTab : public QWidget {

  QTableView* record_table_view;

public:

  RecordTab(QWidget* p);

  void setModel(RecordListModel* model);

  QTableView* tableView() { return record_table_view; }
};



#endif // _WLTOOL_GUI_RECORDTAB_H

