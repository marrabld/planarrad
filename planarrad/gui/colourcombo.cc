
#include "colourcombo.h"



JColour ColourCombo::line_colour[ColourCombo::line_colour_count] = {
  JColour(255,0,0),
  JColour(0,200,0),
  JColour(0,0,255),
  JColour(200,200,0),
  JColour(0,200,200),
  JColour(200,0,200),
  JColour(100,100,0),
  JColour(100,0,100),
  JColour(0,100,100),
  JColour(100,100,100) };



ColourCombo::ColourCombo(QWidget* p) : 
  QComboBox(p) {

  const int colour_icon_size = 14;

  for (int i=0; i<line_colour_count; i++) {
    QPixmap pix(colour_icon_size, colour_icon_size);
    pix.fill(QColor(line_colour[i].rgba()));
    addItem(QIcon(pix), "");
  }
}

