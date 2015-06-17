
// raddirec.h

#include "rad/raddirec.h"

using namespace jude::rad;


// RadDirec::read() [virtual]

bool RadDirec::read(JDataStream& str, uns32 saved_version) { 
  if (saved_version >= VERSION_CODE(2, 1, 0)) {
    int32 n;
    str >> n;
    band_count = n;
  } 
  int32 d;
  str >> d;
  data_type = (datatype) d; 
  return true; 
}



// RadDirec::read() [virtual]

bool RadDirec::write(JDataStream& str) const { 
  str << ((int32) band_count);
  str << ((int32) data_type); 
  return true; 
}


