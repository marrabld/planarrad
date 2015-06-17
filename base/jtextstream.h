
// jtextstream.h

#ifndef _JUDE_BASE_JTEXTSTREAM_H
#define _JUDE_BASE_JTEXTSTREAM_H

#include "base/jtypes.h"
#include "base/jiodevice.h"
#include "base/jstring.h"
#include <vector>

namespace jude {
namespace base {



class JTextStream {

  JIODevice* dev;

  int prec;
  uns32 flags;

public:

  //static const int scientific; // 1
  //static const int fixed;      // 2

  enum { scientific=1, fixed=2 };

  JTextStream();
  JTextStream(JIODevice* d);
  virtual ~JTextStream() { }

  JIODevice& device() { return *dev; }
  void setDevice(JIODevice* d) { dev = d; }

  void setFixed(int dp) { prec=dp; flags=fixed; }
  void setScientific(int dp) { prec=dp; flags=scientific; }

  virtual int precision() const { return prec; }
  virtual int precision(int p) { return (prec=p); }

  virtual int setf(uns32 f);
  virtual int unsetf(uns32 f);

  bool atEnd() const;

  virtual JTextStream& operator<<(int v);
  virtual JTextStream& operator<<(unsigned int v);
  virtual JTextStream& operator<<(int64 v);
  virtual JTextStream& operator<<(double d);
  virtual JTextStream& operator<<(const JString& s);
  virtual JTextStream& operator<<(const char* s);

  template <typename T>
  JTextStream& operator<<(const std::vector<T>& v) {
    (*this) << "[ ";
    for (unsigned int i=0; i<v.size(); i++) { (*this) << v[i]; if (i<v.size()-1) (*this) << ", "; }
    (*this) << " ]";
    return *this;
  }

  virtual void printf(const char* format, ...);

  JString readLine();
};



template <typename T> 
inline bool writeToTextStream(const T& v, JTextStream& str) { return v.write(str); }



} } // namespace jude::base

#endif // _JUDE_BASE_JTEXTSTREAM_H

