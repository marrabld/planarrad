
#ifndef _WLTOOL_GUI_MERGETAB_H
#define _WLTOOL_GUI_MERGETAB_H

#include <QWidget>

class MergeListModel;
class QTableView;



// MergeTab

class MergeTab : public QWidget {

  QTableView* merge_table_view;

public:

  MergeTab(QWidget* p);

  void setModel(MergeListModel* model);

  QTableView* tableView() { return merge_table_view; }
};



#endif // _WLTOOL_GUI_MERGETAB_H

