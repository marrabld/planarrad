
#include <qapplication.h>
#include <qpushbutton.h>



int main(int argc, char *argv[]) {

  QApplication app(argc, argv);

  QPushButton hello("Hello world!", 0);
  
  hello.show();
  return app.exec();
}

