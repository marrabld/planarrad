
// jdatastream.cc

#include "jdatastream.h"
#include "string.h"

using namespace jude::base;



// JDataStream::operator >> uns32

JDataStream& JDataStream::operator>>(uns32& i) {
  dev->readBlock((char*)&i, 4);
  return *this;
}



// JDataStream::operator >> uns16

JDataStream& JDataStream::operator>>(uns16& i) {
  dev->readBlock((char*)&i, 2);
  return *this;
}



// JDataStream::operator >> uns8

JDataStream& JDataStream::operator>>(uns8& i) {
  dev->readBlock((char*)&i, 1);
  return *this;
}



// JDataStream::operator >> int32

JDataStream& JDataStream::operator>>(int32& i) {
  dev->readBlock((char*)&i, sizeof(int32));
  return *this;
}



// JDataStream::operator >> int64

JDataStream& JDataStream::operator>>(int64& i) {
  dev->readBlock((char*)&i, 8);
  return *this;
}



// JDataStream::operator >> int16

JDataStream& JDataStream::operator>>(int16& i) {
  dev->readBlock((char*)&i, 2);
  return *this;
}



// JDataStream::operator >> int8

JDataStream& JDataStream::operator>>(int8& i) {
  dev->readBlock((char*)&i, 1);
  return *this;
}




// JDataStream::operator >> float

JDataStream& JDataStream::operator>>(float32& f) {
  dev->readBlock((char*)&f, sizeof(float32));
  return *this;
}



// JDataStream::operator >> double

JDataStream& JDataStream::operator>>(double64& d) {
  dev->readBlock((char*)&d, sizeof(double64));
  return *this;
}



// JDataStream::operator >> void*

JDataStream& JDataStream::operator>>(bool& f) {
  dev->readBlock((char*)&f, sizeof(bool));
  return *this;
}



// JDataStream::operator >> void*

JDataStream& JDataStream::operator>>(void*& p) {
  dev->readBlock((char*)&p, sizeof(void*));
  return *this;
}



// JDataStream::operator << uns32

JDataStream& JDataStream::operator<<(uns32 i) {
  dev->writeBlock((char*)&i, 4);
  return *this;
}



// JDataStream::operator << uns16 

JDataStream& JDataStream::operator<<(uns16 i) {
  dev->writeBlock((char*)&i, 2);
  return *this;
}


// JDataStream::operator << uns8

JDataStream& JDataStream::operator<<(uns8 i) {
  dev->writeBlock((char*)&i, 1);
  return *this;
}



// JDataStream::operator << int32

JDataStream& JDataStream::operator<<(int32 i) {
  dev->writeBlock((char*)&i, sizeof(int32));
  return *this;
}



// JDataStream::operator << int64

JDataStream& JDataStream::operator<<(int64 i) {
  dev->writeBlock((char*)&i, 8);
  return *this;
}



// JDataStream::operator << int16 

JDataStream& JDataStream::operator<<(int16 i) {
  dev->writeBlock((char*)&i, 2);
  return *this;
}


// JDataStream::operator << int8

JDataStream& JDataStream::operator<<(int8 i) {
  dev->writeBlock((char*)&i, 1);
  return *this;
}



// JDataStream::operator << float32

JDataStream& JDataStream::operator<<(float32 f) {
  dev->writeBlock((char*)&f, sizeof(float32));
  return *this;
}



// JDataStream::operator << double64

JDataStream& JDataStream::operator<<(double64 d) {
  dev->writeBlock((char*)&d, sizeof(double64));
  return *this;
}



// JDataStream::operator << void*

JDataStream& JDataStream::operator<<(bool f) {
  dev->writeBlock((char*)&f, sizeof(bool));
  return *this;
}



// JDataStream::operator << const char*

JDataStream& JDataStream::operator<<(const char* t) {
  dev->writeBlock((char*)t, strlen(t));
  return *this;
}



// JDataStream::operator << void*

JDataStream& JDataStream::operator<<(void* p) {
  dev->writeBlock((char*)&p, sizeof(void*));
  return *this;
}



// JDataStream::readBytes()

JDataStream& JDataStream::readBytes(char*& s, uns32& len) {
  dev->readBlock((char*)&len, 4);
  s = new char[len];
  dev->readBlock(s, len);
  return *this;
}



// JDataStream::readRawBytes()

JDataStream& JDataStream::readRawBytes(char* s, uns32 len) {
  dev->readBlock(s, len);
  return *this;
}



// JDataStream::writeBytes()

JDataStream& JDataStream::writeBytes(const char *s, uns32 len) {
  dev->writeBlock((char*)&len, 4);
  dev->writeBlock(s, len);
  return *this;
} 



// JDataStream::writeRawBytes()

JDataStream& JDataStream::writeRawBytes(const char *s, uns32 len) {
  dev->writeBlock(s, len);
  return *this;
} 

