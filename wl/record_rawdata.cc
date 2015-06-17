
#include "wl/record_rawdata.h"
#include "wl/sensor_rawdata.h"

using namespace jude::wl;



// Record_RawData::(constructor)

Record_RawData::Record_RawData(const Sensor_RawData* s) :
  Record(s), 
  packet_bytes(0),
  byte_count(0) { }



// Record_RawData::(destructor)

Record_RawData::~Record_RawData() {
  delete[] packet_bytes;
}



// Record_RawData::appendBytes()

void Record_RawData::appendBytes(const uns8*& ptr, int& bytes_left) {

    // alloctate space to hold the bytes in this packet
  byte_count = bytes_left;
  packet_bytes = new uns8[byte_count];

    // copy in the bytes
  uns8* dest_ptr(packet_bytes);
  for (; bytes_left>0; bytes_left--) *(dest_ptr++) = *(ptr++);
  
    // for RawData we don't know the underlying record structure so each packet contribution is just one record
  setIsComplete(true);
}



// Record_RawData::writeBytes()

void Record_RawData::writeBytes(JDataStream& str) const {

  uns8* ptr(packet_bytes);
  uns8* ptr_end(packet_bytes + byte_count);

  for (; ptr<ptr_end; ptr++)  str << *ptr;
}



void Record_RawData::writeProcessedHeader(JTextStream& str) const {
  str << "UNPROCESSED_RAWDATA";
}



// Record_RawData::writeProcessed()

void Record_RawData::writeProcessed(JTextStream& str) const {
  str << "UNPROCESSED_RAWDATA";
}



const char* Record_RawData::textDisplayLine(int i) const {
  return 0;
}

