
// specload.h

#ifndef _JUDE_FE_SPECLOAD_H
#define _JUDE_FE_SPECLOAD_H

#include "fe/fileload.h"

namespace jude { namespace rad { class BandSpec; } }

using namespace jude::rad;

namespace jude {
namespace fe {

class BandSpecComboBox;



class SpecLoad : public FileLoad {

  BandSpecComboBox* bandspec_cm;
 
  //JString temp_filepath;

public:
 
  SpecLoad(const JString& text, QWidget* parent, QVBoxLayout* vbox, bool stretch_hbox=true, const JString& ct="Load File");
  SpecLoad(const JString& text, QWidget* parent, QGridLayout* grid, int row, int start_col=0, const JString& ct="Load File");

  void rebuild();

  void setEnabled(bool f);

  void setSourceFilepath(const JString& fp);
  void setBandSpecFilepath(const JString& fp);

  const JString sourceFilepath(); 
  const JString bandSpecFilepath();

  //const JString compatibleFilepath(BandSpec& bs);
};



} } // namespace jude::fe

#endif // _JUDE_FE_SPECLOAD_H

