
#include "wl/record_ctd.h"
#include "wl/sensor_ctd.h"
#include "base/jlog.h"
#include <stdio.h>

using namespace jude::wl;
using namespace jude::base;



// Record_CTD::(constructor)

Record_CTD::Record_CTD(const Sensor_CTD* s) :
  Record_TEXT(s) { }



// Record_CTD::(destructor)

Record_CTD::~Record_CTD() {
  // nothing to do
}



// Record_CTD::buildRecord()

bool Record_CTD::buildRecord(int& final_pos) {

  int bytes_read;
  if (!decompose(bytes_read)) return false;

  setIsComplete(true);
  setHasProblem((item_list.count() != 4));
 
  final_pos = total_length - bytes_read; 

    // don't need the data blocks anymore
  //clearDataBlocks();

  if (jlog::vbLevel(5)) display();

  return true;
}



void Record_CTD::writeProcessedHeader(JTextStream& str) const {

  //const Sensor_CTD* sens = dynamic_cast<const Sensor_CTD*>(sensor()); 

  //if (!sens->customProcessedHeader().isEmpty()) { str << sens->customProcessedHeader(); return; }

  str << "temp\tcond\tpres\tsaln";
}



// Record_CTD::processedColumnsCount()

int Record_CTD::processedColumnsCount() const {
  return 4;
}



// Record_CTD::processedHeader()

const JString Record_CTD::processedHeader(int i) const {
  switch (i) {
    case 0 : return "temp";
    case 1 : return "cond";
    case 2 : return "pres";
    case 3 : return "saln";
  }
  return "";
}



// Record_CTD::textDisplayLine()

const char* Record_CTD::textDisplayLine(int i) const {

  static const int bufflen = 100;
  static char buffer[bufflen];

  switch (i) {
  case 0 : snprintf(buffer, bufflen, "<b>temperature</b> = %s", tempertureString().latin1()); break;
  case 1 : snprintf(buffer, bufflen, "<b>conductivity</b> = %s", conductivityString().latin1()); break;
  case 2 : snprintf(buffer, bufflen, "<b>pressure</b> = %s", pressureString().latin1()); break;
  case 3 : snprintf(buffer, bufflen, "<b>salinity</b> = %s", salinityString().latin1()); break;
  case 4 : snprintf(buffer, bufflen, "<br>[<b>is_complete</b> = %d]", isComplete()); break;
  case 5 : snprintf(buffer, bufflen, "[<b>has_problem</b> = %d]", hasProblem()); break;
 
  default: return 0;
  }

  return buffer;
}

