
// buttongroup.cc

#include "fe/buttongroup.h"
#include "fe/button.h"

using namespace jude::fe;



// ButtonGroup::add()

void ButtonGroup::add(Button* b) {
  button_list.append(b);
}



// ButtonGroup::down()

void ButtonGroup::down(Button* b_down) {

  Button* b = button_list.first();

  while (b) {
    if (b != b_down && b->isDown()) {
      b->setDown(false);
      b->repaint(false);
    }
    b = button_list.next();
  }
}

