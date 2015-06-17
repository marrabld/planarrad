
#ifndef _JUDE_WL_TEXTUTILS_H
#define _JUDE_WL_TEXTUTILS_H

#include <stdio.h>
 
namespace jude {
namespace wl {



class TextUtils {

public:

  static bool readLine(char *buff, int buff_len, FILE* f);
  static bool onlyWhiteSpace(char *buff, int buff_len);
  static void stripToChar(char *buff, int buff_len, char *dest, char c);
  static void stripToWhiteSpace(char *buff, int buff_len, char *dest);
  static void displayChars(char *buff, int buff_len);

};



} } // namespace jude::wl

#endif // _JUDE_WL_TEXTUTILS_H

