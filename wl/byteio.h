
#ifndef _BYTEIO_JDH
#define _BYTEIO_JDH

#include <stdio.h>
#include "base/jtypes.h"
#include "bytestream.h"

namespace jude {
namespace wl {



inline void byteWrite1(int v, FILE* f) {
  fputc(v & 255, f);
}

inline void byteWrite2BE(int v, FILE* f) {
  fputc((v>>8) & 255, f);
  fputc(v & 255, f);
}

inline void byteWrite3BE(int v, FILE* f) {
  fputc((v>>16) & 255, f);
  fputc((v>>8) & 255, f);
  fputc(v & 255, f);
}

inline void byteWrite4BE(int v, FILE* f) {
  fputc((v>>24) & 255, f);
  fputc((v>>16) & 255, f);
  fputc((v>>8) & 255, f);
  fputc(v & 255, f);
}



inline void byteRead1(int& v, FILE* f) {
  v = fgetc(f);
}

inline void byteRead1(unsigned char& c, FILE* f) {
  c = fgetc(f);
}

inline void byteRead2BE(int& v, FILE* f) {
  int b0 = fgetc(f);
  int b1 = fgetc(f);
  v = (b0<<8) + b1;
}


inline void byteRead2BE(uns16& v, const uns8*& ptr) {
  int b0 = *(ptr++);
  int b1 = *(ptr++);
  v = (b0<<8) + b1;
}


inline void byteRead2SE(int16& v, const uns8*& ptr) {
  int b0 = *(ptr++);
  int b1 = *(ptr++);
  v = (b1<<8) + b0;
}


inline void byteRead2BE(uns16& v, FILE* f) {
  int b0 = fgetc(f);
  int b1 = fgetc(f);
  v = (b0<<8) + b1;
}

inline void byteRead2SE(short& v, FILE* f) {
  int b0 = fgetc(f);
  int b1 = fgetc(f);
  v = (b1<<8) + b0;
}

inline void byteRead3BE(int& v, FILE* f) {
  int b0 = fgetc(f);
  int b1 = fgetc(f);
  int b2 = fgetc(f);
  v = (b0<<16) + (b1<<8) + b2;
}

inline void byteRead4BE(int& v, FILE* f) {
  int b0 = fgetc(f);
  int b1 = fgetc(f);
  int b2 = fgetc(f);
  int b3 = fgetc(f);
  v = (b0<<24) + (b1<<16) + (b2<<8) + b3;
}

//inline void byteRead4BE(int32& v, FILE* f) {
//  int b0 = fgetc(f);
// int b1 = fgetc(f);
//  int b2 = fgetc(f);
// int b3 = fgetc(f);
// v = (b0<<24) + (b1<<16) + (b2<<8) + b3;
//}


inline void byteRead4SE(int32& v, FILE* f) {
  int b0 = fgetc(f);
  int b1 = fgetc(f);
  int b2 = fgetc(f);
  int b3 = fgetc(f);
  v = (b3<<24) + (b2<<16) + (b1<<8) + b0;
}




inline void byteRead1(uns8& v, ByteStream& bs) {
  v = bs.read();
}

inline void byteRead2BE(uns16& v, ByteStream& bs) {
  int b0 = bs.read();
  int b1 = bs.read();
  v = (b0<<8) + b1;
}

inline void byteRead3BE(uns32& v, ByteStream& bs) {
  int b0 = bs.read();
  int b1 = bs.read();
  int b2 = bs.read();
  v = (b0<<16) + (b1<<8) + b2;
}

inline void byteRead4BE(uns32& v, ByteStream& bs) {
  int b0 = bs.read();
  int b1 = bs.read();
  int b2 = bs.read();
  int b3 = bs.read();
  v = (b0<<24) + (b1<<16) + (b2<<8) + b3;
}




inline int byteAdd1(int v) {
  return (v & 255);
}

inline int byteAdd2(int v) {
  return ((v>>8) & 255) + (v & 255);
}

inline int byteAdd3(int v) {
  return ((v>>16) & 255) + ((v>>8) & 255) + (v & 255);
}

inline int byteAdd4(int v) {
  return ((v>>24) & 255) + ((v>>16) & 255) + ((v>>8) & 255) + (v & 255);
}



} } // namespace jude::wl

#endif // _BYTEIO_JDH

