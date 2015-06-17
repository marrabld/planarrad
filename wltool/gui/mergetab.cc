
#include "mergetab.h"
#include "mergelistmodel.h"
#include <QTableView>
#include <QLayout>



// MergeTab::(constructor)

MergeTab::MergeTab(QWidget* p) : 
  QWidget(p) {

  //QVBoxLayout* vl = new QVBoxLayout(this, GuiDefaults::viewMargin(), GuiDefaults::viewSpacing());
  QVBoxLayout* vl = new QVBoxLayout(this, 4, 4);
  merge_table_view = new QTableView(this);
  merge_table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  vl->addWidget(merge_table_view);
}



// MergeTab::setModel()

void MergeTab::setModel(MergeListModel* model) {
  merge_table_view->setModel(model);
}

