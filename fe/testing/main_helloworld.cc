
// main_helloworld.cc

#include <qapplication.h>
#include <qpushbutton.h>

#include "base/jmutex.h"

using namespace jude::base;


int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  QPushButton hello("Hello world!", 0);

  JMutex test;
  test.lock();

#if QT_MAJOR < 4
  app.setMainWidget(&hello);
#endif
  
  hello.show();
  return app.exec();
}

