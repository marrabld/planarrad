
#include "textutils.h"
#include <stdio.h>

using namespace jude::wl;



// TextUtils::readLine() [static]

bool TextUtils::readLine(char *buff, int buff_len, FILE* f) {

  if (feof(f)) return false;

  char c;
  int i = 0;

  bool trim_wsp = true;

  while (!feof(f) && i<buff_len-1) {
    if (fread(&c,1,1,f) != 1) break;
    if (c==0 || c==13 || c==10) break;
    if ((c!=' ' && c!=9) || !trim_wsp) { buff[i++] = c;  trim_wsp = false; }
  } 

  buff[i] = 0;
  
  return true;
}



// TextUtils::onlyWhiteSpace() [static]

bool TextUtils::onlyWhiteSpace(char *buff, int buff_len) {
  for (int i=0;i<buff_len;i++) {
    if (buff[i]==0) break;
    if (buff[i]!=' ' && buff[i]!=9 && buff[i]!=13 && buff[i]!=10) return false;
  }
  return true;
}



// TextUtils::stripToChar() [static]

void TextUtils::stripToChar(char *buff, int buff_len, char* dest, char c) {

  char* to = buff;
  char* from = buff;
 
  bool copying = false;

  while (buff_len) {
    if (copying) *(to++) = *from;
    else {
      if (*from == c) copying = true;
      else if (dest) *(dest++) = *from;
    }
    if (*from==0) break;
    from++;
    buff_len--;
  }

  if (dest) *(dest++) = 0;
}



// TextUtils::stripToChar() [static]

void TextUtils::stripToWhiteSpace(char *buff, int buff_len, char* dest) {

  char* to = buff;
  char* from = buff;
 
  bool copying = false;

  while (buff_len) {
    if (copying) *(to++) = *from;
    else {
      if (*from==' ' || *from==9) copying = true;
      else if (dest) *(dest++) = *from;
    }
    if (*from==0) break;
    from++;
    buff_len--;
  }

  if (dest) *(dest++) = 0;
}



void TextUtils::displayChars(char *buff, int buff_len) {
  int i = 0;
  while (i<buff_len) {
    char c = buff[i];
    printf("[%d]",c);
    if (c==0) break;
    i++;
  }
}

