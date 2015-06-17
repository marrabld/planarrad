
// bandspeccombobox.h

#ifndef _JUDE_FE_BANDSPECCOMBOBOX_H
#define _JUDE_FE_BANDSPECCOMBOBOX_H

#include <qcombobox.h>
#include "base/jstring.h"

namespace jude { namespace rad { class BandSpec; } } 

using namespace jude::base;
using namespace jude::rad;

namespace jude {
namespace fe {



class BandSpecComboBox : public QComboBox {

  bool has_default;

public:

  BandSpecComboBox(bool has_default, QWidget* parent);

  void rebuild();

  BandSpec* currentBandSpec();
  const JString currentBandSpecFilepath();

  void setCurrentBandSpecFromFilepath(const JString& fp);
};



} } // namespace jude::fe

#endif // _JUDE_FE_BANDSPECCOMBOBOX_H

