
// raddirecdirec.cc

#include "raddirecdirec.h"

using namespace jude::rad;



bool RadDirecDirec::read(JDataStream& str, uns32 saved_version) {
  if (saved_version >= VERSION_CODE(2, 1, 0)) {
    int32 n;
    str >> n;
    band_count = n;
  } 
  int idt, edt; 
  str >> idt >> edt;
  in_data_type = (datatype) idt;
  ex_data_type = (datatype) edt;
  return true; 
}



bool RadDirecDirec::write(JDataStream& str) const { 
  str << ((int32) band_count);
  str << ((int) in_data_type) << ((int) ex_data_type); 
  return true; 
}

