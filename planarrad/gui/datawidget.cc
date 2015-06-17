
// datawidget.cc

#include "datawidget.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QHeaderView>
#include "base/jlog.h"



DataWidget::DataWidget(DataListModel& data_model, QWidget* parent) :
  data_model(data_model) {

  QVBoxLayout* vl = new QVBoxLayout(this, 4, 4);

  table_view = new QTableView(this);
  table_view->setModel(&data_model);
  table_view->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_view->setSelectionMode(QAbstractItemView::SingleSelection);
  vl->addWidget(table_view);

  table_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table_view->horizontalHeader()->setResizeMode(13, QHeaderView::Stretch);

  QHBoxLayout* hl = new QHBoxLayout(vl);
  load_pb = new QPushButton(" Load ", this);
  connect(load_pb, SIGNAL(clicked()), this, SLOT(slotLoad()));
  hl->addWidget(load_pb);

  hl->addStretch(100);

  clear_pb = new QPushButton(" Unselect ", this);
  clear_pb->setEnabled(false);
  connect(clear_pb, SIGNAL(clicked()), this, SLOT(slotUnselect()));
  hl->addWidget(clear_pb);

  remove_pb = new QPushButton(" Remove Selected ", this);
  remove_pb->setEnabled(false);
  connect(remove_pb, SIGNAL(clicked()), this, SLOT(slotRemove()));
  hl->addWidget(remove_pb);

  connect(table_view->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), 
	  this, SLOT(slotSelectionChanged(const QItemSelection&, const QItemSelection&)));
}



void DataWidget::slotLoad() {

  if (jlog::vb(5)) jlog::ls << "DataWidget::slotLoad()\n";

  QString s = QFileDialog::getOpenFileName(this, tr("Load Data Set"), current_dir_qs);
  JString fp(Q2J(s));

  if (fp.isEmpty()) return;

  DataSet* data = new DataSet(fp);
  if (!data->isOk()) { 
    delete data;
    emit signalReportError(); 
    return; 
  }

  data_model.addDataSet(*data);

  table_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table_view->horizontalHeader()->setResizeMode(13, QHeaderView::Stretch);

  emit signalDataSetsChanged();
}



void DataWidget::autoLoad(const JString& fp) {

  DataSet* data = new DataSet(fp);
  if (!data->isOk()) { 
    delete data;
    return; 
  }

  data_model.addDataSet(*data);

  table_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table_view->horizontalHeader()->setResizeMode(13, QHeaderView::Stretch);

  emit signalDataSetsChanged();
}



void DataWidget::slotRemove() {

  const QModelIndexList& ml(table_view->selectionModel()->selectedRows());
  if (ml.count() == 0) return;

  int r = ml.first().row();

  data_model.removeDataSet(r);

  table_view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  table_view->horizontalHeader()->setResizeMode(13, QHeaderView::Stretch);

  emit signalDataSetsChanged();
}




void DataWidget::slotUnselect() {
  table_view->clearSelection();
}

  

void DataWidget::slotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected) {
  bool f = (table_view->selectionModel()->selectedRows().count() > 0);
  remove_pb->setEnabled(f);
  clear_pb->setEnabled(f);
}

