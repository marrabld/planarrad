
#include "wlchart.h"



// WLChart::(constructor)

WLChart::WLChart(QWidget *p) : 
  BufferWidget(p),
  key(0) { }
 


// WLChart::sizePolicy()

QSizePolicy WLChart::sizePolicy() const {
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}



// WLChart::resizeEvent()
 
void WLChart::resizeEvent(QResizeEvent* e) { 
  // jlog::ls << "WLChart resize: " << width() << " " << height() << "\n"; 
  BufferWidget::resize(width(), height());
  drawToBuffer();
}



// WLChart::slotNewRecordList()

void WLChart::slotNewRecordList(const RecordList* rec_list) {
  newRecordList(rec_list);
}



// WLChart::slotNewRecordSelected()

void WLChart::slotNewRecordSelected(const Record* rec) {
  newRecordSelected(rec);
}



// WLChart::setShowKey()

void WLChart::setShowKey(bool f) {
  chart.setKey((f) ? key : 0);
  chart.arrangeDrawing(buffer().width(), buffer().height());
  chart.drawToBuffer(buffer());
}



// WLChart::drawToBuffer()

void WLChart::drawToBuffer() {
  chart.arrangeDrawing(buffer().width(), buffer().height());
  chart.drawToBuffer(buffer());
}

