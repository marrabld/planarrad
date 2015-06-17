
// buttongroup.h

#ifndef _JUDE_FE_BUTTONGROUP_H
#define _JUDE_FE_BUTTONGROUP_H

#include "base/jlist.h"

using namespace jude::base;

namespace jude {
namespace fe {

class Button;



class ButtonGroup {

  JList<Button> button_list;

public:

  ButtonGroup() { }

  void add(Button* b);

  void down(Button* b_down);
};



} } // namespace jude::fe

#endif // _JUDE_FE_BUTTONGROUP_H



