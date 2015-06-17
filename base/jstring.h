
// jstring.h

#ifndef _JUDE_BASE_JSTRING_H
#define _JUDE_BASE_JSTRING_H

#include "base/jtypes.h"
#include "base/jchar.h"
#include "base/jdatastream.h"

namespace jude {
namespace base {



/*! \class JString
\brief For representation and manipulation of text strings.

The JString type is a general string representation. It is the basis of all text manipulation within the Jude library. Text is stored as 16-bit unicode.

\todo Reading and writing of JString to JTextStream seems to be an obvious omission. 

\todo Build some test cases for conversion between numbers and text and for chopping functions.
*/



// JString is supposed to be byte-compatible with Qt's QString
// these macros enable conversion both ways

#define JSTRING_TO_QSTRING(js) QString((QChar*)(js).unicode(),(js).length())
#define QSTRING_TO_JSTRING(qs) JString((JChar*)(qs).unicode(),(qs).length())

#define J2Q(js) JSTRING_TO_QSTRING(js)
#define Q2J(qs) QSTRING_TO_JSTRING(qs)



class JString {

  int len;
  JChar* array;

  mutable char* latin1_text;
  mutable bool update_buffers;

  void allocate(int n);

public:

  JString();
  JString(const char* text);
  JString(const char* text, unsigned int n);
  JString(const JChar* u);
  JString(const JChar* u, unsigned int n);
  JString(const JString& s);
  JString(JDataStream& str);
 
  ~JString();
  
  bool isEmpty() const;
  unsigned int length() const { return len; }
  const JChar* unicode() const { return array; }
  JChar at(int i) const { return array[i]; }

  int contains(JChar c) const;

  const char* latin1() const;

  JString& append(const JString& s);
  JString& prepend(const JString& s);
  JString section(JChar sep, int start, int end) const;
  
  JString left(unsigned int n) const;
  JString right(unsigned int n) const;

  JString stripWhiteSpace() const;
  JString upper() const;
  JString lower() const;
 
  int toInt() const;
  int toInt(int start_pos, int end_pos) const;

  double toDouble() const;
  double toDouble(int start_pos, int end_pos) const;

  static JString number(double v, const char c='g', int prec=2);
  static JString number(int v);
  static JString number(int64 v);

  static const JString null;
 
  const JString& operator=(const char* text);
  const JString& operator=(const JString& s);

  const JString& operator+=(const char* text);
  const JString& operator+=(const JString& s);

  friend const JString operator+(const JString& s1, const JString &s2);

  friend bool operator==(const JString& s1, const JString &s2);
  friend bool operator!=(const JString& s1, const JString &s2);

  friend JDataStream& operator<<(JDataStream& str, const JString& s);
  friend JDataStream& operator>>(JDataStream& str, JString& s);
};



const JString operator+(const JString& s1, const JString &s2);

bool operator==(const JString& s1, const JString &s2);
bool operator!=(const JString& s1, const JString &s2);

JDataStream& operator<<(JDataStream& str, const JString& s);
JDataStream& operator>>(JDataStream& str, JString& s);

template <>
inline bool writeToDataStream<JString>(const JString& v, JDataStream& str) { str << v; return true; }  


template <>
inline bool readFromDataStream<JString>(JString& v, JDataStream& str, uns32) { str >> v; return true; }


} } // namespace jude::base



#endif // _JUDE_BASE_JSTRING_H

