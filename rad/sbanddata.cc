
// sbanddata.cc

#include "rad/sbanddata.h"

using namespace jude::rad;



/*! Reads the spectral data from a text stream. Returns true no error occured, */
 
bool SBandData::readFromTextStream(JTextStream& str) {
  return readFromTextStream(data, str);
}



/*! Writes the data out to the text stream \str if a form suitable for console or log file output. */

void SBandData::display(JTextStream& str) const {
  str.setScientific(JUDE_DEFAULT_OUTPUT_PREC);
  str << data;
}

