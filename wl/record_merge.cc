
#include "record_merge.h"
#include "base/jlog.h"

using namespace jude::wl;




// Record_MERGE::(constructor)

Record_MERGE::Record_MERGE() :
  record_array(new const Record*[HARDWARE_CHANNEL_COUNT]),
  time_diff(new long[HARDWARE_CHANNEL_COUNT]) { 

  for (int i=0;i<HARDWARE_CHANNEL_COUNT;i++) {
    record_array[i] = 0;
    time_diff[i] = 0;
  }

  is_complete = true;
  has_problem = false;
  is_excluded = false;
}



// Record_MERGE::(destructor)

Record_MERGE::~Record_MERGE() {
  delete[] record_array;
}



// Record_MERGE::record()

const jude::wl::Record* Record_MERGE::record(int i) const { 
  return (i>=0 && i<HARDWARE_CHANNEL_COUNT) ? record_array[i] : 0; 
}



// Record_MERGE::setRecord()

void Record_MERGE::setRecord(int i, const Record* r, long td) {
  record_array[i] = r;
  time_diff[i] = td;
}




// Record_MERGE::textDisplayLine()

const char* Record_MERGE::textDisplayLine(int i) const {
  return 0;
}




// Record_MERGE::writeProcessedHeader()

void Record_MERGE::writeProcessedHeader(JTextStream& str) const {

  bool done_first_column = false;

  for (int i=0;i<HARDWARE_CHANNEL_COUNT;i++) {
    if (record_array[i]) {
      if (done_first_column) str << "\t";
      else done_first_column = true;
      record_array[i]->writeProcessedHeader(str);
      str << "\tmsec_offset";
    }
  }
}



// Record_MERGE::writeBytesProcessed()

void Record_MERGE::writeProcessed(JTextStream& str) const {

  bool done_first_column = false;

  for (int i=0;i<HARDWARE_CHANNEL_COUNT;i++) {
    if (record_array[i]) {
      if (done_first_column) str << "\t";
      else done_first_column = true;
      record_array[i]->writeProcessed(str);
      str << "\t" << (int) time_diff[i];
    }
  }
}



// Record_MERGE::processedColumnsCount()

int Record_MERGE::processedColumnsCount() const {

  int c = 0;

  for (int i=0;i<HARDWARE_CHANNEL_COUNT;i++) {
    if (record_array[i]) c += record_array[i]->processedColumnsCount() + 1;
  }

  return c;
}



// Record_MERGE::processedHeader()

const JString Record_MERGE::processedHeader(int col) const {

  int i=0;
  for (; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (!record_array[i]) continue;
    if (col<=record_array[i]->processedColumnsCount()) break;
    col -= record_array[i]->processedColumnsCount()+1; 
  }
  if (i == HARDWARE_CHANNEL_COUNT) return "";

  //jlog::ls << "array " << i << " col " << col << "\n";
  if (col >= record_array[i]->processedColumnsCount()) return "msec_offset";
  return record_array[i]->processedHeader(col);
}
 


// Record_MERGE::processedHeader()

const JString Record_MERGE::processed(int col) const {

  int i=0;
  for (; i<HARDWARE_CHANNEL_COUNT; i++) {
    if (!record_array[i]) continue;
    if (col<=record_array[i]->processedColumnsCount()) break;
    col -= record_array[i]->processedColumnsCount()+1; 
  }
  if (i == HARDWARE_CHANNEL_COUNT) return "";

  if (col >= record_array[i]->processedColumnsCount()) return JString::number((int)time_diff[i]);
  return record_array[i]->processed(col);
}
 


// Record_MERGE::display()

void Record_MERGE::display() const {

}

