
#include "jimagesave_testing.h"
#include "base/jlog.h"
#include "base/jimagesave.h"
#include "base/jbuffer.h"
#include "base/jcolour.h"
#include "base/jtypes.h"
#include "base/jfile.h"

using namespace jude::base;



void JImageSave_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;

  JBuffer buff;

  jlog::ls << "Testing image save of cross40x40.ppm... ";
  drawCross(buff, 40, 40, JColour::Black, JColour::White);
  JImageSave::savePPM(buff, "cross40x40.ppm");
  if (JFile::binaryCompare("cross40x40.ppm", "cross40x40_ref.ppm")) { jlog::ls << "Pass\n"; }
  else { jlog::ls << "Fail\n"; fc++; }
  tc++;

  jlog::ls << "Testing image save of cross20x50.ppm... ";
  drawCross(buff, 20, 50, JColour::Red, JColour::Green);
  JImageSave::savePPM(buff, "cross20x50.ppm");
  if (JFile::binaryCompare( "cross20x50.ppm", "cross20x50_ref.ppm")) { jlog::ls << "Pass\n"; }
  else { jlog::ls << "Fail\n"; fc++; }
  tc++;

  jlog::ls << "JImageSave Testing passed " << tc-fc << " of " << tc << " tests.\n\n";
  
  total_count+= tc;
  fail_count += fc;
}



// [private]
// Resizes the \a buff to a square of \a x_size x \a y_size pixels and draws a \a cross_col coloured cross on a \a back_col coloured background.

void JImageSave_Testing::drawCross(JBuffer& buff, int x_size, int y_size, const JColour& back_col, const JColour& cross_col) {

    // resize the buffer
  buff.requestResize(x_size, y_size);
   // clear it to background colour
  buff.fill(back_col);

    // figure out how big a cross can be fitted in
  int cross_size = min(x_size, y_size);

    // write in individual pixels
  for (int p=0; p<cross_size; p++) {
    buff.writePixel(p, p, cross_col);
    buff.writePixel(cross_size-p-1, p, cross_col);
  }
}

