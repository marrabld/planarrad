
#ifndef _WLTOOL_GUI_PACKETTAB_H
#define _WLTOOL_GUI_PACKETTAB_H

#include <QWidget>

class PacketListModel;
class QTableView;



// PacketTab

class PacketTab : public QWidget {

  QTableView* packet_table_view;

public:

  PacketTab(QWidget* p);

  void setModel(PacketListModel* mod);

  QTableView* tableView() { return packet_table_view; }
};



#endif // _WLTOOL_GUI_PACKETTAB_H

