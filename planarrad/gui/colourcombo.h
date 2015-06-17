
// colourcombo.h

#ifndef _PLANARRAD_COLOURCOMBO_H
#define _PLANARRAD_COLOURCOMBO_H

#include <QComboBox>
#include "base/jcolour.h"

using namespace jude::base;



class ColourCombo : public QComboBox {

  enum { line_colour_count = 10 };
  static JColour line_colour[line_colour_count];

public:

  ColourCombo(QWidget* p);

  JColour lineColour() const { return line_colour[currentIndex()]; }
};




#endif // _PLANARRAD_COLOURCOMBO_H


