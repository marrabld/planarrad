
// jdatastream.h

#ifndef _JUDE_BASE_JDATASTREAM_H
#define _JUDE_BASE_JDATASTREAM_H

#include "base/jtypes.h"
#include "base/jiodevice.h"
#include <vector>

namespace jude {
namespace base {



// JDataStream

class JDataStream {

  JIODevice* dev;

public:

  JDataStream(JIODevice* d) : dev(d) { }

  JIODevice& device() { return *dev; }
  void setDevice(JIODevice* d) { dev = d; }

  JDataStream& operator>>(uns32& i);
  JDataStream& operator>>(uns16& i);
  JDataStream& operator>>(uns8& i);
  JDataStream& operator>>(int64& i);
  JDataStream& operator>>(int32& i);
  JDataStream& operator>>(int16& i);
  JDataStream& operator>>(int8& i);
  JDataStream& operator>>(float32& f);
  JDataStream& operator>>(double64& d);
  JDataStream& operator>>(bool& f);
  JDataStream& operator>>(void*& p);

  JDataStream& operator<<(uns32 i);
  JDataStream& operator<<(uns16 i);
  JDataStream& operator<<(uns8 i);
  JDataStream& operator<<(int64 i);
  JDataStream& operator<<(int32 i);
  JDataStream& operator<<(int16 i);
  JDataStream& operator<<(int8 i);
  JDataStream& operator<<(float32 f);
  JDataStream& operator<<(double64 d);
  JDataStream& operator<<(bool f);
  JDataStream& operator<<(const char* t);
  JDataStream& operator<<(void* p);

  template <typename T> 
  JDataStream& operator>>(std::vector<T>& v) {
    uns32 n;
    (*this) >> n;
    v.resize(n);
    for (unsigned int i=0; i<n; i++) (*this) >> v[i];
    return *this;
  } 
    
  template <typename T> 
  JDataStream& operator<<(const std::vector<T>& v) {
    (*this) << (uns32) v.size();
    for (unsigned int i=0; i<v.size(); i++) (*this) << v[i];
    return *this;
  } 
    
  //template <typename T> 
  //void write(const std::vector<T>& v) {
  //  (*this) << (uns32) v.size();
  // for (int i=0; i<v.size(); i++) (*this) << v[i];
  //}  

  JDataStream& readBytes(char*& s, uns32& len); 
  JDataStream& readRawBytes(char* s, uns32 len); 

  JDataStream& writeBytes(const char *s, uns32 len); 
  JDataStream& writeRawBytes(const char *s, uns32 len); 

  void byteRead1(int& v); 
  void byteRead1(unsigned char& c);
  void byteRead2BE(int& v);
  void byteRead2BE(uns16& v); 
  void byteRead2SE(short& v);
  void byteRead3BE(int& v);
  void byteRead4BE(int& v);
  void byteRead4SE(int32& v);

  void byteWrite1(const int& v); 
  void byteWrite1(const unsigned char& c);
  void byteWrite2BE(const int& v);
  void byteWrite2BE(const uns16& v); 
  void byteWrite2SE(const short& v);
  void byteWrite3BE(const int& v);
  void byteWrite4BE(const uns32& v);
  void byteWrite4SE(const int32& v);
};



template <typename T> 
inline bool writeToDataStream(const T& v, JDataStream& str) { return v.write(str); }

template <>
inline bool writeToDataStream<int>(const int& v, JDataStream& str) { str << v; return true; }  

template <>
inline bool writeToDataStream<double>(const double& v, JDataStream& str) { str << v; return true; }  

template <typename T> 
inline bool readFromDataStream(T& v, JDataStream& str, uns32 saved_version) { return v.read(str, saved_version); }

template <>
inline bool readFromDataStream<int>(int& v, JDataStream& str, uns32) { str >> v; return true; }

template <>
inline bool readFromDataStream<double>(double& v, JDataStream& str, uns32) { str >> v; return true; }



inline void JDataStream::byteRead1(int& v) {
  v = dev->getch();
}


inline void JDataStream::byteRead1(unsigned char& c) {
  c = dev->getch();
}


inline void JDataStream::byteRead2BE(int& v) {
  int b0 = dev->getch();
  int b1 = dev->getch();
  v = (b0<<8) + b1;
}


inline void JDataStream::byteRead2BE(uns16& v) {
  int b0 = dev->getch();
  int b1 = dev->getch();
  v = (b0<<8) + b1;
}


inline void JDataStream::byteRead2SE(short& v) {
  int b0 = dev->getch();
  int b1 = dev->getch();
  v = (b1<<8) + b0;
}


inline void JDataStream::byteRead3BE(int& v) {
  int b0 = dev->getch();
  int b1 = dev->getch();
  int b2 = dev->getch();
  v = (b0<<16) + (b1<<8) + b2;
}


inline void JDataStream::byteRead4BE(int& v) {
  int b0 = dev->getch();
  int b1 = dev->getch();
  int b2 = dev->getch();
  int b3 = dev->getch();
  v = (b0<<24) + (b1<<16) + (b2<<8) + b3;
}


inline void JDataStream::byteRead4SE(int32& v) {
  int b0 = dev->getch();
  int b1 = dev->getch();
  int b2 = dev->getch();
  int b3 = dev->getch();
  v = (b3<<24) + (b2<<16) + (b1<<8) + b0;
}



inline void JDataStream::byteWrite1(const int& v) {
  dev->putch(v);
}


inline void JDataStream::byteWrite1(const unsigned char& c) {
  dev->putch(c);
}


inline void JDataStream::byteWrite2BE(const int& v) {
  int b0 = (v & 0xff00) >> 8;
  int b1 = (v & 0xff);
  dev->putch(b0);
  dev->putch(b1);
}


inline void JDataStream::byteWrite2BE(const uns16& v) {
  int b0 = (v & 0xff00) >> 8;
  int b1 = (v & 0xff);
  dev->putch(b0);
  dev->putch(b1);
}


inline void JDataStream::byteWrite2SE(const short& v) {
  int b0 = (v & 0xff);
  int b1 = (v & 0xff00) >> 8;
  dev->putch(b0);
  dev->putch(b1);
}


inline void JDataStream::byteWrite3BE(const int& v) {
  int b0 = (v & 0xff0000) >> 16;
  int b1 = (v & 0xff00) >> 8;
  int b2 = (v & 0xff);
  dev->putch(b0);
  dev->putch(b1);
  dev->putch(b2);
}


inline void JDataStream::byteWrite4BE(const uns32& v) {
  int b0 = (v & 0xff000000) >> 24;
  int b1 = (v & 0xff0000) >> 16;
  int b2 = (v & 0xff00) >> 8;
  int b3 = (v & 0xff);
  dev->putch(b0);
  dev->putch(b1);
  dev->putch(b2);
  dev->putch(b3);
}


inline void JDataStream::byteWrite4SE(const int32& v) {
  int b0 = (v & 0xff);
  int b1 = (v & 0xff00) >> 8;
  int b2 = (v & 0xff0000) >> 16;
  int b3 = (v & 0xff000000) >> 24;
  dev->putch(b0);
  dev->putch(b1);
  dev->putch(b2);
  dev->putch(b3);
}



} } // namespace jude::base

#endif // _JUDE_BASE_JDATASTREAM_H

