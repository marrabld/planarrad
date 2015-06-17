
// jiodevicestore.h

#ifndef _JUDE_BASE_JIODEVICESTORE_H
#define _JUDE_BASE_JIODEVICESTORE_H

#include "base/jiodevice.h"
#include "base/jstring.h"
#include <vector>


namespace jude {
namespace base {



class JIODeviceStore : public JIODevice {

  std::vector<char> data_array;
 
  mutable JString curr_text;
 
public:

  JIODeviceStore() { }
  ~JIODeviceStore() { }

  void clear();

    /*! Open the device for reading or writing dependent on the value of \c mode. */
  bool open(int mode) { return true; }
    /*! Close the device for reading or writing. */
  void close() { }

    /*! \brief Read \c len characters from the device from the array at \c data. Returns the number of characters successfully read.*/
  int32 readBlock(char *data, uns32 len) { return 0; }
    /*! \brief Write \c len characters to the device from the array at \c data. Returns the number of characters successfully written.*/
  int32 writeBlock(const char *data, uns32 len);
    /*! \brief Write characters to the device from the array at \c data until a zero character is encountered (which is not written).
    Returns the number of characters successfully written. */
  int32 writeBlockUntilZero(const char *data);

    /*! \brief Read a character. Returns the value of the character read. */
  int getch() { return 0; }
    /*! \brief Write a character. Returns the value of the character written or -1 if failed. */
  int putch(int ch);

    /*! \brief Subclass should physically write out any cached data. */
  void flush() { }

    /*! \brief Subclass must return \c true if the end of file or similar has been reached. */
  virtual bool atEnd() const { return false; }

  const JString& currentText() const;
};



} } // namespace jude::base

#endif // _JUDE_BASE_JIODEVICESTORE_H

