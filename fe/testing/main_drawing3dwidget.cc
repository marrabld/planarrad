
// main_drawing3dwidget.cc

#include <qapplication.h>
#include "fe/drawing3dwidget.h"
#include "drawS3/drawing3d.h"
#include "drawS3/line3d.h"
#include "base/jlog.h"

using namespace jude::fe;
using namespace jude::drawS3;



int main(int argc, char *argv[]) {

  JLog::init();

  QApplication app(argc, argv);

  Drawing3D d;
  Drawing3DWidget w(0);
  w.attachDrawing(&d);
  w.setFixedSize(200, 200);

  d.setBackgroundColour(JColour::Black);

  Box3D box(Vec3D(-1,-1,-1), Vec3D(1,1,1));

  d.addItem(new Line3D(box.v000(), box.v001()));
  d.addItem(new Line3D(box.v001(), box.v011()));
  d.addItem(new Line3D(box.v011(), box.v010()));
  d.addItem(new Line3D(box.v010(), box.v000()));

  d.addItem(new Line3D(box.v100(), box.v101()));
  d.addItem(new Line3D(box.v101(), box.v111()));
  d.addItem(new Line3D(box.v111(), box.v110()));
  d.addItem(new Line3D(box.v110(), box.v100()));

  d.addItem(new Line3D(box.v000(), box.v100()));
  d.addItem(new Line3D(box.v001(), box.v101()));
  d.addItem(new Line3D(box.v010(), box.v110()));
  d.addItem(new Line3D(box.v011(), box.v111()));

#if QT_MAJOR < 4
  app.setMainWidget(&w);
#endif
  
  w.show();
  return app.exec();
}

