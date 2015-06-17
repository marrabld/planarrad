
#include "wl/record_ecobb3.h"
#include "wl/sensor_ecobb3.h"
#include "base/jlog.h"

using namespace jude::wl;



// Record_ECOBB3::(constructor)

Record_ECOBB3::Record_ECOBB3(const Sensor_ECOBB3* s) :
  Record_TEXT(s) { }



// Record_ECOBB3::(destructor)

Record_ECOBB3::~Record_ECOBB3() {
  // nothing to do
}



// Record_ECOBB3::buildRecord()

bool Record_ECOBB3::buildRecord(int& final_pos) {

  int bytes_read;
  if (!decompose(bytes_read)) return false;

  setIsComplete(true);

  final_pos = total_length - bytes_read; 

    // don't need the data blocks anymore
  //clearDataBlocks();

  if (jlog::vbLevel(5)) display();

  return true;
}



void Record_ECOBB3::writeProcessedHeader(JTextStream& str) const {

  //const Sensor_ECOBB3* sens = dynamic_cast<const Sensor_ECOBB3*>(sensor()); 

  //if (!sens->customProcessedHeader().isEmpty()) { str << sens->customProcessedHeader(); return; }

  str << "date\ttime\t470_ref\t470_sig\t530_ref\t530_sig\t650_ref\t650_sig\ttemp";
}



int Record_ECOBB3::processedColumnsCount() const {
  return 9;
}



const JString Record_ECOBB3::processedHeader(int i) const {
  switch (i) {
    case 0 : return "data";
    case 1 : return "time";
    case 2 : return "470_ref";
    case 3 : return "470_sig";
    case 4 : return "530_ref";
    case 5 : return "530_sig";
    case 6 : return "650_ref";
    case 7 : return "650_sig";
    case 8 : return "temp";
  }
  return "";
}



// Record_ECOBB3::textDisplayLine()

const char* Record_ECOBB3::textDisplayLine(int i) const {

  static const int bufflen = 100;
  static char buffer[bufflen];

  switch (i) {
  case 0 : snprintf(buffer, bufflen, "<b>date</b> = %s", dataString().latin1()); break;
  case 1 : snprintf(buffer, bufflen, "<b>time</b> = %s", timeString().latin1()); break;
  case 2 : snprintf(buffer, bufflen, "<b>470_ref</b> = %d", ref470()); break;
  case 3 : snprintf(buffer, bufflen, "<b>470_sig</b> = %d", sig470()); break;
  case 4 : snprintf(buffer, bufflen, "<b>530_ref</b> = %d", ref530()); break;
  case 5 : snprintf(buffer, bufflen, "<b>530_sig</b> = %d", sig530()); break;
  case 6 : snprintf(buffer, bufflen, "<b>650_ref</b> = %d", ref650()); break;
  case 7 : snprintf(buffer, bufflen, "<b>650_sig</b> = %d", sig650()); break;
  case 8 : snprintf(buffer, bufflen, "<b>thermistor</b> = %d", therm()); break;
  case 9 : snprintf(buffer, bufflen, "<br>[<b>is_complete</b> = %d]", isComplete()); break;
  case 10 : snprintf(buffer, bufflen, "[<b>has_problem</b> = %d]", hasProblem()); break;
  
  default: return 0;
  }

  return buffer;
}

