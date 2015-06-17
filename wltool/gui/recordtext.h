
#ifndef _WLTOOL_GUI_RECORDTEXT_H
#define _WLTOOL_GUI_RECORDTEXT_H

#include <QTextEdit>
#include "wl/record.h"

using namespace jude::wl;



// RecordText

class RecordText : public QTextEdit {

  Q_OBJECT

public:

  RecordText(QWidget* p);

public slots:

  void slotNewRecordSelected(const Record* rec);
};



#endif //_WLTOOL_GUI_RECORDTEXT_H

