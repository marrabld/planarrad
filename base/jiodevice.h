
// jiodevice.h

#ifndef _JUDE_BASE_JIODEVICE_H
#define _JUDE_BASE_JIODEVICE_H

#include "base/jtypes.h"

namespace jude {
namespace base {




//enum { IO_Read = 1,
//       IO_Write = 2 };

enum { JIO_ReadOnly = 1, 
       JIO_WriteOnly = 2,
       JIO_ReadWrite = 3,
       JIO_AppendOnly = 4,
       JIO_ReadAppend = 5 };


/*!
\brief Interface for a general input-ouput device such as a file or a text console.

JTextStream and JDataStream write to a JIODevice. 
So the best way to redirect or manipluate the output of those classes is to subclass JIODevice.
*/

class JIODevice {

public:

  JIODevice() { }
  virtual ~JIODevice() { }

    /*! Open the device for reading or writing dependent on the value of \c mode. */
  virtual bool open(int mode) = 0;
    /*! Close the device for reading or writing. */
  virtual void close() = 0;

    /*! \brief Read \c len characters from the device from the array at \c data. Returns the number of characters successfully read.*/
  virtual int32 readBlock(char *data, uns32 len) = 0;
    /*! \brief Write \c len characters to the device from the array at \c data. Returns the number of characters successfully written.*/
  virtual int32 writeBlock(const char *data, uns32 len) = 0;
    /*! \brief Write characters to the device from the array at \c data until a zero character is encountered (which is not written).
    Returns the number of characters successfully written. */
  virtual int32 writeBlockUntilZero(const char *data) = 0;

    /*! \brief Read a character. Returns the value of the character read. */
  virtual int getch() = 0;
    /*! \brief Write a character. Returns the value of the character written or -1 if failed. */
  virtual int putch(int ch) = 0;

    /*! \brief Subclass should physically write out any cached data. */
  virtual void flush() = 0;

    /*! \brief Move the cursor up \a n lines. The default is to do nothing. Only expected to implemented for consoles and the like. */
  virtual void cursorUp(int n) { /* default is nop */ }

    /*! \brief Subclass must return \c true if the end of file or similar has been reached. */
  virtual bool atEnd() const = 0;
};



} } // namespace jude::base

#endif // _JUDE_BASE_JIODEVICE_H

