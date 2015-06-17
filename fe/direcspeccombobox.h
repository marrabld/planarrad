
// direcspeccombobox.h

#ifndef _JUDE_FE_DIRECSPECCOMBOBOX_H
#define _JUDE_FE_DIRECSPECCOMBOBOX_H

#include <qcombobox.h>
#include "base/jstring.h"

namespace jude { namespace ds { class DirecSpec; } } 

using namespace jude::base;
using namespace jude::ds;

namespace jude {
namespace fe {



class DirecSpecComboBox : public QComboBox {

public:

  DirecSpecComboBox(bool has_default, QWidget* parent);

  DirecSpec* currentDirecSpec();
  const JString currentDirecSpecFilepath();

  void rebuild();

  void setCurrentDirecSpecFromFilepath(const JString& fp);
};



} } // namespace jude::fe

#endif // _JUDE_FE_DIRECSPECCOMBOBOX_H

