
#include "drawing2d_testing.h"
#include "base/jlog.h"
#include "base/jimagesave.h"
#include "base/jbuffer.h"
#include "base/jcolour.h"
#include "base/jtypes.h"
#include "base/jfile.h"
#include "drawS2/lineseq2d.h"

using namespace jude::drawS2;


// [private]

void Drawing2D_Testing::makeTriangle(Drawing2D& drawing) {

  Vec2D v0(0,10);
  Vec2D v1(10,-5);
  Vec2D v2(-10,-5);

  drawing.deleteAllItems();
  LineSeq2D* ls = new LineSeq2D(v0, v1, v2, JColour::White);
  ls->setClosed(true);
  drawing.addItem(ls);

  drawing.setBackgroundColour(JColour::Black); 
}



// [private]
// Resizes the \a buff to a square of \a x_size x \a y_size pixels and draws a \a cross_col coloured cross on a \a back_col coloured background.

void Drawing2D_Testing::drawDrawing(JBuffer& buff, int x_size, int y_size, const Drawing2D& drawing) {

  Trans2D trans;

    // resize the buffer
  buff.requestResize(x_size, y_size);
    // auto set translation
  trans.autoSet(drawing.boundingBox(), buff.width(), buff.height());
  trans.display(jlog::ls);
    // draw from the view
  drawing.draw(trans, buff);
}



void Drawing2D_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;

  JBuffer buff;
  Drawing2D drawing;
  Trans2D trans;

  jlog::ls << "Testing image create of triangle40x40.ppm... ";
  makeTriangle(drawing);
  drawDrawing(buff, 40, 40, drawing);
  JImageSave::savePPM(buff, "triangle40x40.ppm");

  //if (JFile::binaryCompare("cross40x40.ppm", "cross40x40_ref.ppm")) { jlog::ls << "Pass\n"; }
  //else { jlog::ls << "Fail\n"; fc++; }
  tc++;

  //jlog::ls << "Testing image save of cross20x50.ppm... ";
  //drawCross(buff, 20, 50, JColour::Red, JColour::Green);
  //JImageSave::savePPM(buff, "cross20x50.ppm");
  //if (JFile::binaryCompare( "cross20x50.ppm", "cross20x50_ref.ppm")) { jlog::ls << "Pass\n"; }
  //else { jlog::ls << "Fail\n"; fc++; }
  //tc++;

  jlog::ls << "Drawing2D Testing passed " << tc-fc << " of " << tc << " tests.\n\n";
  
  total_count+= tc;
  fail_count += fc;
}



