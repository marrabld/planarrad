
#include "base/jimagesave.h"
#include "base/jlog.h"
#include "base/jfile.h"
#include "base/jdatastream.h"

using namespace jude::base;



bool JImageSave::savePPM(const JBuffer& buff, const JString& filepath) {

    // check the buffer has a size
  if (buff.width()==0 || buff.height()==0) {
    jlog::es << "Cannat save image because JBuffer has zero size\n.";
    return false;
  }

    // open the file
  JFile f(filepath.latin1());
    // check opened ok
  if (!f.open(JIO_WriteOnly)) {
    jlog::es << "Couldn't open the file '" << filepath << "' for writing.\n";
    return false;
  }

    // get a data stream from the file
  JDataStream str(&f);
  
  int w = buff.width();
  int h = buff.height();

  const JString w_str = JString::number(w);
  const JString h_str = JString::number(h);
  
  str << "P6\n";
  str << (w_str+" "+h_str).latin1() << "\n";
  str << "255\n";

  uns32* data_ptr = buff.data();

  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {

      str << JColour::red(*data_ptr);
      str << JColour::green(*data_ptr);
      str << JColour::blue(*data_ptr);

      data_ptr++;
    }
  }

  f.close();

  return true;
}

