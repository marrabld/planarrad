
// jutil.h

#ifndef _JUDE_BASE_JUTIL_H
#define _JUDE_BASE_JUTIL_H

#include "base/jtypes.h"
#include "base/jfile.h"
#include "base/jtextstream.h"
#include "base/jlog.h"

namespace jude {
namespace base {


class JUtil {

public:

  static const JString reportTime(uns32 msec) {
      // default precision
    int prec = 1;
      // seconds
    double v = msec / 1000.0;
    if (v < 60) {
      if (v < 10) prec = 2;
      if (v < 3) prec = 3;
      return JString::number(v, 'f', prec) + " secs";
    }
      // minutes
    v = v / 60;
    if (v < 60) return JString::number(v, 'f', 2) + " mins";
      // hours
    return JString::number(v/60, 'f', 2) + " hours";
  }
};



/*! \brief Returns the number of bits and bit mask required to code number up to \a size (ono-inclusive) */

static inline void bitsAndMask(int size, uns32& bits_count, uns32& mask) {
  if ((--size)==0) { bits_count = 0; mask = 0; return; }
  bits_count = 1;
  mask = (~0) << 1;
  while ((size >> bits_count) != 0) { bits_count++; mask <<= 1; }
  mask = ~mask;
}



/*! Read a std::vector from a text file.

\todo Set up test cases.
*/

template <typename T>
bool readFromTextFile(std::vector<T>& v, const JString& filepath) {

  v.clear();

    // open the file
  JFile f(filepath.latin1());

  if (!f.open(JIO_ReadOnly)) {
    jlog::es << "File '" << filepath << "' not found.\n"; 
    return false; 
  }

    // get a data stream on the file
  JTextStream str(&f);   

  JString line;

    // read in first line
  do { line = str.readLine().stripWhiteSpace(); } 
  while (line.left(1)=="#");

    // try to find a seperator to use
  JChar sep(' ');
  if (line.contains(JChar(',')) != 0) sep = JChar(',');
  if (line.contains(JChar('\t')) != 0) sep = JChar('\t');

    // if found a seperator then read values as seperated list
  if (line.contains(sep) != 0) {

    int num = line.contains(sep) + 1;
    v.reserve(num);    
    for (int i=0; i<num; i++) v.push_back((T) line.section(sep, i, i).toDouble());

    // otherwise assume each line is a value
  } else {

    do {
        // skip commented lines
      if (line.stripWhiteSpace().left(1) == "#") continue;
        // convert as double then cast
      v.push_back((T) line.toDouble());
        // get next line
      line = str.readLine();

      // continue while read something or not at the end of the file
    } while (!line.isEmpty() || !str.atEnd());
  }

    // close file
  f.close();

  return true;
}



} } // namespace jude::base

#endif // _JUDE_BASE_JUTIL_H

