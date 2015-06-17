
#include "packettab.h"
#include "packetlistmodel.h"
#include <QTableView>
#include <QLayout>



// RecordTab::(constructor)

PacketTab::PacketTab(QWidget* p) : 
  QWidget(p) {

  QVBoxLayout* vl = new QVBoxLayout(this, 4, 4);
  packet_table_view = new  QTableView(this);
  packet_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  vl->addWidget(packet_table_view);
}



// RecordTab::setModel()

void PacketTab::setModel(PacketListModel* mod) {
  packet_table_view->setModel(mod);
}



//  RecordTab::selectionChanged(_

//void PacketTab::selectionChanged(QListViewItem* item) {
  /*
  const RecordItem* ri = dynamic_cast<const RecordItem*>(item);
  const Record* rec = (ri) ? &ri->record() : 0;
  //const Record* rec = 0;
  emit selectedRecord(rec);
  */ 
//}

