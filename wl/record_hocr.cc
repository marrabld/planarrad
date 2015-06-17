
#include "wl/record_hocr.h"
#include "wl/sensor_hocr.h"
#include "base/jlog.h"
#include <string.h>

using namespace jude::wl;



Record_HOCR::Record_HOCR(const Sensor_HOCR* s) :
  Record(s), 
  it(UNKNOWN),
  curr_pos(6),
  bytes_read_count(0),
  has_processed_ed_and_lu(false),
  has_processed_es(false) { }



Record_HOCR::~Record_HOCR() { 
  // nothing to do
}



const JString Record_HOCR::typeText() const { 
  JString s(sens->typeText());
  switch (it) {
    case SATHPE : s+=" (Ed)"; break;
    case SATPED : s+=" (Ed Dark)"; break;
    case SATHPL : s+=" (Lu)"; break;
    case SATPLD : s+=" (Lu Dark)"; break;
    default : s+=" (?)";
  }
  return s;
}



void Record_HOCR::appendBytes(const uns8*& ptr, int& bytes_left) {

  //jlog::ls << "here " << bytes_left << " it " << it << "\n";

    // if haven't yet established the type of this record then cycle bytes through the 6-byte type name buffer
  while (it == UNKNOWN && bytes_left > 0) {

      // shift bytes through by one and new one onto the end
    for (int i=0; i<5; i++) data_bytes[i] = data_bytes[i+1];
    data_bytes[5] = *(ptr++);
      // read one more byte
    bytes_read_count++;
    bytes_left--;
      // get if we can now can start the record
    if (!strncmp((char*)data_bytes, "SATHPE", 6)) it = SATHPE;
    else if (!strncmp((char*)data_bytes, "SATPED", 6)) it = SATPED;
    else if (!strncmp((char*)data_bytes, "SATHPL", 6)) it = SATHPL;
    else if (!strncmp((char*)data_bytes, "SATPLD", 6)) it = SATPLD;
      // for debugging
    //if (it != UNKNOWN) { data_bytes[6] = 0; jlog::ls.printf("found: %s\n",data_bytes); }
  }

    // keep filling the record until got to the end or run out of bytes
  while (curr_pos < 427 && bytes_left > 0) {
    data_bytes[curr_pos++] = *(ptr++); 
    bytes_read_count++;
    bytes_left--;
  }

    // note if finished the record 
  if (curr_pos == 427) {
    setIsComplete(true);
    time_secs = JString((const char*)(data_bytes+414), 10).toDouble();
    //jlog::ls << "bytes_read_count " << bytes_read_count << " time: " << JString((const char*)(data_bytes+414), 10).latin1() << "\n"; 
  }
}



// Record_HOCR::writeBytes()
// for Prosoft to read the Satlantic sensor data an additional 7-byte time stamp must be added to each record
// this is described in the document: SAT-DN-00507_HyperOCData_Logger_Integration_Data_Format_Requirements.pdf

void Record_HOCR::writeBytes(JDataStream& str) const {

    // write out the 427 data bytes
  for (int i=0; i<427; i++) str << data_bytes[i];

    // now write out the 3-byte DATETAG which is the decimal YYYYDDD written msb first
    // we don't know the real date and it doens't matter so write out 2000000
  uns32 day = 2000000;
  str << (uns8) ((day & 0xff0000) >> 16);
  str << (uns8) ((day & 0xff00) >> 8);
  str << (uns8) (day & 0xff);

    // now write out the 4-byte TIMETAG2 which is the decimal HHMMSSmmm
  int64 msec = loggerTimeMsec();
    // seperate out the hours
  int hours = msec / 3600000;
  msec -= hours * 3600000;
    // seperate out the minutes
  int mins = msec / 60000;
  msec -= mins * 60000;
    // seperate out the seconds
  int secs = msec / 1000;
  msec -= secs * 1000;
    // build the number as a string
  char buff[11];
  sprintf(buff,"%02d%02d%02d%03d",hours,mins,secs,(int)msec);
  buff[10] = 0;
  JString s(buff);
  uns32 v = s.toInt();
  str.byteWrite4BE(v);
}



void Record_HOCR::writeProcessedHeader(JTextStream& str) const {

  if (!has_processed_ed_and_lu && !has_processed_es) {
    str << "UNPROCESSED_HOCR";
    return;
  }

  const Sensor_HOCR* sens_hocr = dynamic_cast<const Sensor_HOCR*>(sensor()); 

  if (!sens_hocr->doEdOutput() && !sens_hocr->doLuOutput() && !sens_hocr->doEsOutput()) {
    str << "EXCLUDED_HOCR";
    return;
  }

  int prec = str.precision();
  str.setFixed(2);
  
  if (sens_hocr->doEdOutput() && has_processed_ed_and_lu) {
    str << "Ed_timetag\t";
    for (int i=0; i<sens_hocr->bandCount(); i++) {
      str << JString("Ed") << sens_hocr->EdBandWavelength()(i);
      if (i < sens_hocr->bandCount()-1 || sens_hocr->doLuOutput()) str << "\t";
    }
  }

  if (sens_hocr->doLuOutput() && has_processed_ed_and_lu) {
    str << "Lu_timetag\t";
    for (int i=0; i<sens_hocr->bandCount(); i++) {
      str << JString("Lu") << sens_hocr->LuBandWavelength()(i); 
      if (i < sens_hocr->bandCount()-1 || sens_hocr->doEsOutput()) str << "\t";
    }
  }
  
  if (sens_hocr->doEsOutput() && has_processed_es) {
    str << "Es_timetag\t";
    for (int i=0; i<sens_hocr->esBandCount(); i++) {
      str << JString("Es") << sens_hocr->EsBandWavelength()(i); 
      if (i < sens_hocr->esBandCount()-1) str << "\t";
    }
  }

  str.setScientific(prec);
}



void Record_HOCR::writeProcessed(JTextStream& str) const {

  if (!has_processed_ed_and_lu && !has_processed_es) { str << "UNPROCESSED_HOCR"; return; }

  const Sensor_HOCR* sens_hocr = dynamic_cast<const Sensor_HOCR*>(sensor()); 
  if (!sens_hocr) return;

  if (!sens_hocr->doEdOutput() && !sens_hocr->doLuOutput() && !sens_hocr->doEsOutput()) {
    str << "EXCLUDED_HOCR";
    return;
  }

  if (sens_hocr->doEdOutput() && has_processed_ed_and_lu) {
    str << ed_timetag2 << "\t";
    for (int i=0; i<sens_hocr->bandCount(); i++) {
      str << ed_data(i);
      if (i < sens_hocr->bandCount()-1 || sens_hocr->doLuOutput()) str << "\t";
    }
  }

  if (sens_hocr->doLuOutput() && has_processed_ed_and_lu) {
    str << lu_timetag2 << "\t";
    for (int i=0; i<sens_hocr->bandCount(); i++) {
      str << lu_data(i);
      if (i < sens_hocr->bandCount()-1 || sens_hocr->doEsOutput()) str << "\t";
    }
  }

  if (sens_hocr->doEsOutput() && has_processed_es) {
    str << es_timetag2 << "\t";
    for (int i=0; i<sens_hocr->esBandCount(); i++) {
      str << es_data(i);
      if (i < sens_hocr->esBandCount()-1) str << "\t";
    }
  }
}



void Record_HOCR::clearProcessed() {
  ed_data.resize(0);
  lu_data.resize(0);
  es_data.resize(0);
  has_processed_ed_and_lu = false;
  has_processed_es = false;
}



void Record_HOCR::clearProcessedEdAndLu() {
  ed_data.resize(0);
  lu_data.resize(0);
}



void Record_HOCR::clearProcessedEs() {
  es_data.resize(0);
}



// Record_HOCR::processedColumnsCount()

int Record_HOCR::processedColumnsCount() const {

  if (!isProcessed()) return 1;

  const Sensor_HOCR* sens_hocr = dynamic_cast<const Sensor_HOCR*>(sensor()); 
  if (!sens_hocr) return 0;
 
  if (!sens_hocr->doEdOutput() && !sens_hocr->doLuOutput() && !sens_hocr->doEsOutput()) return 1;
 
  int c = 0;
  if (sens_hocr->doEdOutput() && has_processed_ed_and_lu) c += sens_hocr->bandCount() + 1;
  if (sens_hocr->doLuOutput() && has_processed_ed_and_lu) c += sens_hocr->bandCount() + 1;
  if (sens_hocr->doEsOutput() && has_processed_es) c += sens_hocr->esBandCount() + 1;
 
  return c;
}



// Record_HOCR::processedHeader()

const JString Record_HOCR::processedHeader(int i) const {

  if (!isProcessed()) return "UNPROCESSED_HOCR";
 
  const Sensor_HOCR* sens_hocr = dynamic_cast<const Sensor_HOCR*>(sensor()); 
  if (!sens_hocr) return "";
 
  if (!sens_hocr->doEdOutput() && !sens_hocr->doLuOutput() && !sens_hocr->doEsOutput()) return "EXCLUDED_HOCR";

  bool is_ed = (i < sens_hocr->bandCount()+1 && sens_hocr->doEdOutput());
  if (i >= sens_hocr->bandCount()+1) i -= sens_hocr->bandCount()+1;
  bool is_lu = (i < sens_hocr->bandCount()+1 && sens_hocr->doLuOutput());
  if (i >= sens_hocr->bandCount()+1) i -= sens_hocr->bandCount()+1;

  if (is_ed) {
    if (i==0) return "Ed_timetag";
    if (i >= (int)sens_hocr->EdBandWavelength().size()+1) return "";
    return JString("Ed") + JString::number(sens_hocr->EdBandWavelength()(i-1), 'f', 2);
  }
 
  if (is_lu) {
    if (i==0) return "Lu_timetag";
    if (i >= (int)sens_hocr->LuBandWavelength().size()+1) return "";
    return JString("Lu") + JString::number(sens_hocr->LuBandWavelength()(i-1), 'f', 2);
  }

  if (i==0) return "Es_timetag";
  if (i >= (int)sens_hocr->EsBandWavelength().size()+1) return "";
  return JString("Es") + JString::number(sens_hocr->EsBandWavelength()(i-1), 'f', 2);
}



// Record_HOCR::processed()

const JString Record_HOCR::processed(int i) const {

  if (!isProcessed()) return "UNPROCESSED_HOCR";

  const Sensor_HOCR* sens_hocr = dynamic_cast<const Sensor_HOCR*>(sensor()); 
  if (!sens_hocr) return "";

  if (!sens_hocr->doEdOutput() && !sens_hocr->doLuOutput() && !sens_hocr->doEsOutput()) return "EXCLUDED_HOCR";

  bool is_ed = (i < sens_hocr->bandCount()+1 && sens_hocr->doEdOutput());
  if (i >= sens_hocr->bandCount()+1) i -= sens_hocr->bandCount()+1;
  bool is_lu = (i < sens_hocr->bandCount()+1 && sens_hocr->doLuOutput());
  if (i >= sens_hocr->bandCount()+1) i -= sens_hocr->bandCount()+1;

  if (is_ed) {
    if (i==0) return ed_timetag2;
    if (i >= (int)ed_data.size()+1) return ""; 
    return JString::number(ed_data(i-1),'e',5);
  }

  if (is_lu) {
    if (i==0) return lu_timetag2;
    if (i >= (int)lu_data.size()+1) return "";   
    return JString::number(lu_data(i-1),'e',5);
  }

  if (i==0) return es_timetag2;
  if (i >= (int)es_data.size()+1) return ""; 
  return JString::number(es_data(i-1),'e',5);
}



// Record_HOCR::textDisplayLine()

const char* Record_HOCR::textDisplayLine(int i) const {

  static const int bufflen = 3000;
  static char buffer[bufflen];

  switch (i) {
  case 0 : snprintf(buffer, bufflen, "<b>instrument</b> = %s", instrument().latin1()); break;
  case 1 : snprintf(buffer, bufflen, "<b>serial_no</b> = %s", serialNo().latin1()); break;
  case 2 : snprintf(buffer, bufflen, "<b>int_time</b> = %d", (int)intTime()); break;
  case 3 : snprintf(buffer, bufflen, "<b>sample_delay</b> = %d", (int) sampleDelay()); break;
  case 4 : snprintf(buffer, bufflen, "<b>dark_samp</b> = %u", (int) darkSamp()); break;
  case 5 : snprintf(buffer, bufflen, "<b>dark_ave</b> = %u", (int) darkAverage()); break;
  case 6 : snprintf(buffer, bufflen, "<b>frame_counter</b> = %d", (int) frameCounter()); break;
  case 7 : snprintf(buffer, bufflen, "<b>timer</b> = %s", timer().latin1()); break;
  case 8 : snprintf(buffer, bufflen, "<b>check_sum</b> = %d", (int) checkSum()); break;
  case 9 : dataText("<br><b>raw_data</b> =", buffer, bufflen); break;
  case 10 : snprintf(buffer, bufflen, "<br>[<b>is_complete</b> = %d]", isComplete()); break;
  case 11 : snprintf(buffer, bufflen, "[<b>has_problem</b> = %d]", hasProblem()); break;
  case 12 : snprintf(buffer, bufflen, "[<b>is_processed</b> = %d]", isProcessed()); break;

  default: return 0;
  }

  return buffer;
}





// Record_HOCR::dataText() [private]

void Record_HOCR::dataText(const char* str, char *buff, int buff_len) const {

  int n = snprintf(buff, buff_len, "%s ",str);
  buff += n;
  buff_len -= n;

  for (int i=0;i<rawDataBandCount() && buff_len>0;i++) {
    int n = snprintf(buff, buff_len, "%d ", rawData(i));
    if (n<0) return;
    buff += n;
    buff_len -= n;
  }
}

