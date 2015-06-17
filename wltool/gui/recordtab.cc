
#include "recordtab.h"
#include "recordlistmodel.h"
#include <QTableView>
#include <QLayout>



// RecordTab::(constructor)

RecordTab::RecordTab(QWidget* p) : 
  QWidget(p) {

  QVBoxLayout* vl = new QVBoxLayout(this, 4, 4);
  record_table_view = new QTableView(this);
  record_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  vl->addWidget(record_table_view);
}



// RecordTab::setModel()

void RecordTab::setModel(RecordListModel* model) {
  record_table_view->setModel(model);
}

