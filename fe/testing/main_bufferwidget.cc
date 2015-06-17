
// main_bufferwidget.cc

#include <qapplication.h>
#include "fe/bufferwidget.h"

using namespace jude::fe;



int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  BufferWidget w(0);
  w.setFixedSize(100,100);
  w.buffer().requestResize(100,100);
  w.buffer().fill(JColour::Red);

#if QT_MAJOR < 4
  app.setMainWidget(&w);
#endif  

  w.show();
  return app.exec();
}

