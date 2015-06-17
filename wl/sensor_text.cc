
#include "sensor_text.h"
#include "textutils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace jude::wl;



// Sensor_TEXT::(constructor)

Sensor_TEXT::Sensor_TEXT(int ci) :
  Sensor(ci),
  column_count(0),
  column_head(0) { }



// Sensor_TEXT::(destructor)

Sensor_TEXT::~Sensor_TEXT() {
  deleteColumnData();
}



// Sensor_TEXT::deleteColumnData() [private]

void Sensor_TEXT::deleteColumnData() {
  for (int i=0;i<column_count;i++) delete[] column_head[i];
  delete[] column_head;
  column_head = 0;
}



// Sensor_TEXT::setColumnCount()

void Sensor_TEXT::setColumnCount(int n) {
  deleteColumnData();
  column_count = n;
  column_head = new char*[column_count];
  for (int i=0;i<column_count;i++) column_head[i] = 0;
}



// Sensor_TEXT::setColumnHead()

void Sensor_TEXT::setColumnHead(int i, const char* txt) {
  delete[] column_head[i];
  column_head[i] = new char[strlen(txt)+1];
  strcpy(column_head[i], txt);
}



// Sensor_TEXT::readDeviceFile()

bool Sensor_TEXT::readDeviceFile(const char* filepath) {

  FILE* f = fopen(filepath,"r");
  if (!f) return false;

  const int bufflen = 100;
  char buffer[bufflen];
  char header_text[bufflen];
  char header_num[bufflen];

    // first non-empty line is sensor name
  do {
    if (!TextUtils::readLine(buffer, bufflen, f)) return false;
  } while (TextUtils::onlyWhiteSpace(buffer, bufflen));

    // set sensor name
  setNameText(buffer);

    // find Columns=x line
  do {
    if (!TextUtils::readLine(buffer, bufflen, f)) return false;
  } while (strncmp(buffer, "Columns=", 8) != 0);

    // set number of columns
  TextUtils::stripToChar(buffer, bufflen, 0, '=');
  int col_num = atoi(buffer);
  setColumnCount(col_num);

  int col_done = 0;

    // find column description lines
  do {

      // get non-empty lines
    do { 
      if (!TextUtils::readLine(buffer, bufflen, f)) { fclose(f); return false; } 
    } while (TextUtils::onlyWhiteSpace(buffer, bufflen));

    TextUtils::displayChars(buffer, bufflen);

    TextUtils::stripToChar(buffer, bufflen, header_text, '=');
    TextUtils::stripToWhiteSpace(buffer, bufflen, header_num);
    int i = atoi(header_num);
    printf("%d %s %s %d\n",i,header_text,header_num,col_num);
    setColumnHead(i-1, header_text);
    col_done++;

  } while (col_done < col_num);


  fclose(f);

  return true;
}

