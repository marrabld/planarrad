
#include "wl/sensor_hocr.h"
#include "base/jlog.h"
#include <string.h>

using namespace jude::wl;



void Sensor_HOCR::writeHeaderRecord(const char* data, const char* ident, JDataStream& str) const {

  str << "SATHDR " << data << " (" << ident << ")";

  int zeros_todo = 128 - (10 + strlen(data) + strlen(ident)); 

  for (; zeros_todo>0; zeros_todo--) str << ((char) 0);
}



void Sensor_HOCR::writeHeader(JDataStream& str) const {

  writeHeaderRecord("","CRUISE-ID", str);
  writeHeaderRecord("","OPERATOR", str);
  writeHeaderRecord("","LATITUDE", str);
  writeHeaderRecord("","LONGITUDE", str);
  writeHeaderRecord("","ZONE", str);
  writeHeaderRecord("","COMMENT", str);
  writeHeaderRecord("","STATION-ID", str);
  writeHeaderRecord("","CAST", str);
  writeHeaderRecord("","TIME-STAMP", str);
  writeHeaderRecord("NONE","MODE", str);
  writeHeaderRecord("","PRESSURE-TARE", str);
  writeHeaderRecord("OFF","TIMETAG", str);
  writeHeaderRecord("ON","DATETAG", str);
  writeHeaderRecord("ON","TIMETAG2", str);

  //writeHeaderRecord("OFF","PROFILER", str);
  //writeHeaderRecord("WET","REFERENCE", str);
  //writeHeaderRecord("OFF","PRO-DARK", str);
  //writeHeaderRecord("OFF","REF-DARK", str);
  //writeHeaderRecord("","PRO-ID", str);
}



void Sensor_HOCR::prepareForMerge(const JParser& prm, int chan_index) {
  JString label(JString("chan")+JString::number(channelIndex()+1)+"_hocr_l2s");
  do_ed_output = prm.isDefinedAndTrueBool(label+"_ed_output");
  do_lu_output = prm.isDefinedAndTrueBool(label+"_lu_output");
  do_es_output = prm.isDefinedAndTrueBool(label+"_es_output");
  //jlog::ls << "HERE " << (label+"_ed_output ") << do_ed_output << " " << do_lu_output << "\n";
  prm.displayList(jlog::ls);
}



bool Sensor_HOCR::loadProcessed(const JParser& prm, Channel& chan, int chan_index) {

  if (!loadProcessedEdAndLu(prm, chan, chan_index)) return false;
  if (!loadProcessedEs(prm, chan, chan_index)) return false;

  return true;
}



bool Sensor_HOCR::loadProcessedEdAndLu(const JParser& prm, Channel& chan, int chan_index) {

  stop_processing = false;
  pct_records_processed = 10;

  JString label(JString("chan")+JString::number(chan_index+1)+"_hocr_l2s_ed_and_lu");

    // record for later
  //do_ed_output = prm.isDefinedAndTrueBool(label+"_ed_output");
  //do_lu_output = prm.isDefinedAndTrueBool(label+"_lu_output");

  JString fp = prm.getStringOrEmpty(label+"_fp");

    // if not defined then thats ok
  if (fp.isEmpty()) return true;

  if (jlog::vb(1)) jlog::ls << "Loading processed Ed and Lu data file for channel " << chan_index+1 << " '" << fp << "'\n";

  int row_count;
  int max_cols;
  bool is_consistent;

  JTextTable::getSize(row_count, max_cols, is_consistent, fp, '\t', false);

  JString msg(JString("File '") + fp + "' has a formatting problem so could not be read.");

  pct_records_processed = 20;

    // table is not big enough, must be some problem
  if (row_count < 20) { jlog::es << msg; return false; }

  if (jlog::vbLevel(3)) jlog::ls << "Rows: " << row_count << " Columns: " << max_cols << "\n"; 

  JTextTable ttab(max_cols);
  ttab.setSeperator('\t');
  ttab.readFromFile(fp);

    // start of ed data
  int ed_row_start = 0;
  while (ttab.getText(ed_row_start, 0) != "Datetag" && ed_row_start < row_count) ed_row_start++;
  if (ed_row_start == row_count) { jlog::es << msg << " Specifically, no line beginning 'Datetag' found.\n"; return false; }

    // end of ed data
  int ed_row_end = ed_row_start+1;
  while (ttab.getText(ed_row_end, 0).left(2)!="LU" && ed_row_end < row_count) ed_row_end++;
  int ed_row_count = ed_row_end - ed_row_start;
 
    // start of lu data
  int lu_row_start = ed_row_end+1;
  while (ttab.getText(lu_row_start, 0) != "Datetag" && lu_row_start < row_count) lu_row_start++;
  if (lu_row_start == row_count) { jlog::ls << msg << " Specifically, second line beginning 'Datetag' for Lu data not found.\n"; return false; }
 
    // end of lu data
  int lu_row_end = lu_row_start+1;
  while (!ttab.getText(lu_row_end, 0).stripWhiteSpace().isEmpty() && lu_row_end < row_count) lu_row_end++;
  int lu_row_count = lu_row_end - lu_row_start;
 
  band_count = max_cols - 3;

  if (jlog::vb(5)) {
    jlog::ls << "Ed data start row: " << ed_row_start << " row_count: " << ed_row_count << "\n";
    jlog::ls << "Lu data start row: " << lu_row_start << " row_count: " << lu_row_count << "\n";
    jlog::ls << "Band count: " << band_count << "\n";
  }

  ed_band_wavelength.resize(band_count);
  lu_band_wavelength.resize(band_count);

  for (int i=0; i<band_count; i++) {
    ed_band_wavelength(i) = ttab.getText(ed_row_start, i+2).toDouble();
    lu_band_wavelength(i) = ttab.getText(lu_row_start, i+2).toDouble();
  }
 
  if (jlog::vb(3)) {
    jlog::ls << "Ed wavelengths:\n" << ed_band_wavelength << "\n";
    jlog::ls << "Lu wavelengths:\n" << lu_band_wavelength << "\n";
  }

  int ed_light_count = 0;
  int ed_dark_count = 0;
  int lu_light_count = 0;
  int lu_dark_count = 0;

  int curr_ed_row = ed_row_start+1;
  int curr_lu_row = lu_row_start+1;

  int rec_count = 0;
 
  JListIterator<Record> iter(chan.recordList());
  Record* rec = iter.first();
  while (rec) {

    if (stop_processing) return false;

    Record_HOCR* rec_hocr = dynamic_cast<Record_HOCR*>(rec);

    rec_hocr->edData().resize(band_count);
    rec_hocr->luData().resize(band_count);

    for (int i=0; i<band_count; i++) {
      rec_hocr->edData()(i) = ttab.getText(curr_ed_row, i+2).toDouble();
      rec_hocr->luData()(i) = ttab.getText(curr_lu_row, i+2).toDouble();
    }
    rec_hocr->setEdTimetag2(ttab.getText(curr_ed_row, 1));
    rec_hocr->setLuTimetag2(ttab.getText(curr_lu_row, 1));
    rec_hocr->setProcessedEdTimeStamp(textTimeToInt(rec_hocr->edTimetag2()));
    rec_hocr->setHasProcessedEdAndLu(true);

    if (jlog::vb(5)) { 
      jlog::ls << "Rec: " << rec_count << " time: '" << rec_hocr->edTimetag2();
      jlog::ls << "' secs: " <<  rec_hocr->processedEdTimeStamp() << "\n";
    }

    if (rec_hocr->isEdLight()) { 
      ed_light_count++;
      if (curr_ed_row < ed_row_end) curr_ed_row++;
      if (curr_lu_row < lu_row_end) curr_lu_row++;
    } 
    else if (rec_hocr->isEdDark()) ed_dark_count++;
    else if (rec_hocr->isLuLight()) lu_light_count++;
    else if (rec_hocr->isLuDark()) lu_dark_count++;

    rec_count++;
    pct_records_processed = 20 + (80.0 * rec_count) / chan.recordList().count();

    rec = iter.next();
  }

  if (jlog::vb(2)) {
    jlog::ls << "Ed light records count: " << ed_light_count << "\n";
    jlog::ls << "Ed dark records count: " << ed_dark_count << "\n";
    jlog::ls << "Lu light records count: " << lu_light_count << "\n";
    jlog::ls << "Lu dark records count: " << lu_dark_count << "\n";
  }

  return true;
}



// Sensor_HOCR::loadProcessedEs()

bool Sensor_HOCR::loadProcessedEs(const JParser& prm, Channel& chan, int chan_index) {

  stop_processing = false;
  pct_records_processed = 10;

  JString label(JString("chan")+JString::number(chan_index+1)+"_hocr_l2s_es");
  JString fp = prm.getStringOrEmpty(label+"_fp");

  if (fp.isEmpty()) return true;

  if (jlog::vb(1)) jlog::ls << "Loading processed Es data file for channel " << chan_index+1 << " '" << fp << "'\n";

  int row_count;
  int max_cols;
  bool is_consistent;

  JTextTable::getSize(row_count, max_cols, is_consistent, fp, '\t', false);

  JString msg(JString("File '") + fp + "' has a formatting problem so could not be read.");

  pct_records_processed = 20;

    // table is not big enough, must be some problem
  if (row_count < 20) { jlog::es << msg; return false; }

  if (jlog::vbLevel(3)) jlog::ls << "Rows: " << row_count << " Columns: " << max_cols << "\n"; 

  JTextTable ttab(max_cols);
  ttab.setSeperator('\t');
  ttab.readFromFile(fp);

    // start of es data
  int es_row_start = 0;
  while (ttab.getText(es_row_start, 0) != "Datetag" && es_row_start < row_count) es_row_start++;
  if (es_row_start == row_count) { jlog::es << msg << " Specifically, no line beginning 'Datetag' found.\n"; return false; }

    // end of ed data
  int es_row_end = es_row_start+1;
  while (es_row_end < row_count) es_row_end++;
  int es_row_count = es_row_end - es_row_start;
 
  es_band_count = max_cols - 3;

  if (jlog::vb(5)) {
    jlog::ls << "Es data start row: " << es_row_start << " row_count: " << es_row_count << "\n";
    jlog::ls << "Band count: " << es_band_count << "\n";
  }

  es_band_wavelength.resize(es_band_count);

  for (int i=0; i<es_band_count; i++) es_band_wavelength(i) = ttab.getText(es_row_start, i+2).toDouble();
 
  if (jlog::vb(3)) jlog::ls << "Es wavelengths:\n" << es_band_wavelength << "\n";

  int curr_es_row = es_row_start+1;
  int curr_time = textTimeToInt(ttab.getText(curr_es_row, 1));
  int next_time = (curr_es_row < es_row_end) ? textTimeToInt(ttab.getText(curr_es_row+1, 1)) : -1; 

  int rec_count = 0;
 
  JListIterator<Record> iter(chan.recordList());
  Record* rec = iter.first();
  while (rec) {

    if (stop_processing) return false;

    Record_HOCR* rec_hocr = dynamic_cast<Record_HOCR*>(rec);

      // move forward through es records until found closest time 
    while (curr_es_row < es_row_end) {
      int td1 = absTimeDiff(rec_hocr->processedEdTimeStamp(), curr_time);
      int td2 = absTimeDiff(rec_hocr->processedEdTimeStamp(), next_time);
      if (td1 < td2) break;
      curr_time = next_time;
      curr_es_row++;
      next_time = (curr_es_row < es_row_end) ? textTimeToInt(ttab.getText(curr_es_row+1, 1)) : -1; 
    }

    if (jlog::vb(5)) { 
      jlog::ls << "rec: " << rec_count << " time: " << rec_hocr->processedEdTimeStamp();
      jlog::ls << " es rec: " << curr_es_row-es_row_start-1 << " time: " << curr_time << " " << next_time << "\n";
    }

    rec_hocr->esData().resize(es_band_count);    
    for (int i=0; i<es_band_count; i++) rec_hocr->esData()(i) = ttab.getText(curr_es_row, i+2).toDouble();
    rec_hocr->setEsTimetag2(ttab.getText(curr_es_row, 1));
    rec_hocr->setHasProcessedEs(true);

    rec_count++;
    pct_records_processed = 20 + (80.0 * rec_count) / chan.recordList().count();

    rec = iter.next();
  }

  return true;
}



void Sensor_HOCR::clearProcessedEdAndLu() {
  ed_band_wavelength.resize(0);
  lu_band_wavelength.resize(0);
}



void Sensor_HOCR::clearProcessedEs() {
  es_band_wavelength.resize(0);
}



// Sensor_HOCR::textTimeToInt()

int Sensor_HOCR::textTimeToInt(const JString& text_time) {
  JString t(text_time.stripWhiteSpace());
  int hours = t.section(':',0,0).toInt();
  int mins = t.section(':',1,1).toInt();
  int secs = t.section(':',2,2).toInt();
  return ((hours * 60) + mins) * 60 + secs; 
}



// Sensor_HOCR::absTimeDiff()

int Sensor_HOCR::absTimeDiff(int t1, int t2) {

  //return abs(t1 - t2);

  const int offset_24hr = 24 * 60 * 60;

  if (t1 > t2) {
    int diff1 = t1 - t2;
    int diff2 = t2 - t1 + offset_24hr;
    return min(diff1, diff2);
  }

  int diff1 = t2 - t1;
  int diff2 = t1 - t2 + offset_24hr;
  return min(diff1, diff2);
}


