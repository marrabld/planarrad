
// jtextdevice.h

#ifndef _JUDE_BASE_JTEXTDEVICE_H
#define _JUDE_BASE_JTEXTDEVICE_H

#include "base/jiodevice.h"
#include "base/jstring.h"

namespace jude {
namespace base {



/*!
\brief Interface for output to JString, so JTextStream can write to JString for example.
*/

class JTextDevice : public JIODevice {

  JString s;

public:
 
    /*! \brief Returns a reference to the text string of the device. */
  JString& text() { return s; }
    /*! \brief Returns a const reference to the text string of the device. */
  const JString& text() const { return s; }

    /*! Open the device for reading or writing dependent on the value of \c mode. */
  bool open(int mode);
    /*! Close the device for reading or writing. */
  void close();

    /*! \brief Read \c len characters from the device from the array at \c data. Returns the number of characters successfully read.*/
  int32 readBlock(char *data, uns32 len);
    /*! \brief Write \c len characters to the device from the array at \c data. Returns the number of characters successfully written.*/
  int32 writeBlock(const char *data, uns32 len);
    /*! \brief Write characters to the device from the array at \c data until a zero character is encountered (which is not written).
    Returns the number of characters successfully written. */
  int32 writeBlockUntilZero(const char *data);

    /*! \brief Read a character. Returns the value of the character read. */
  int getch();
    /*! \brief Write a character. Returns the value of the character written or -1 if failed. */
  int putch(int ch);

    /*! \brief Subclass should physically write out any cached data. */
  void flush();

    /*! \brief Subclass must return \c true if the end of file or similar has been reached. */
  bool atEnd() const;
};



} } // namespace jude::base

#endif // _JUDE_BASE_JTEXTDEVICE_H



