
#include "recordtext.h"
#include <QScrollBar>



// RecordText::(constructor)

RecordText::RecordText(QWidget* p) :
  QTextEdit(p) {

  //setMinimumHeight(GuiDefaults::recordTextMinHeight());
  setMinimumHeight(100);

  setReadOnly(true);
}




// RecordText::setRecord()

void RecordText::slotNewRecordSelected(const Record* rec) {

  if (!isEnabled()) return;

  clear();       

  if (!rec) return;

  int i = 0;

  while (true) {
    const char* line = rec->textDisplayLine(i);
    if (!line) break;
    append(QString(line));
    i++;
  }

  verticalScrollBar()->setSliderPosition(0);
  //setContentsPos(0,0);
}

