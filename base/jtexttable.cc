
// jtexttable.cc

#include "base/jtexttable.h"
#include "base/jfile.h"
#include "base/jtextstream.h"
#include "base/jlog.h"

using namespace jude::base;



// JTextTable::getSize() [static]
// get the size of the table represented by a file without actually reading in any data

bool JTextTable::getSize(int& row_count, int& max_cols, bool& is_consistent, const JString& filepath, JChar sep, bool verbose) {

  is_consistent = true;
  row_count = 0;
  max_cols = 0;
  bool sep_fixed = (sep != 0);
  bool sep_seen = false;
 
    // open the file
  JFile f(filepath);
  if (!f.open(JIO_ReadOnly)) { printf("Couldn't open file '%s'",filepath.latin1()); return false; }
  
    // get a text stream on the file
  JTextStream str(&f);       

  JString line;

  int curr_line = 0;

  while (!f.atEnd()) {
    
      // read next line ignoring comments and empty lines
    do { line = str.readLine(); curr_line++; } 
    while ((line.isEmpty() || line.stripWhiteSpace().left(1)=="#") && !f.atEnd());

    //    jlog::ls << "line " << curr_line << " len " << line.length() << " " << line.contains(sep) << " " << (int) sep << "\n";
    //if (curr_line == 3320) {
    // for (int i=0; i< line.length(); i++) jlog::ls << " " << (int) line.at(i);
    //  jlog::ls << "\n"; 
    // }

    if (f.atEnd()) break;

      // got an extra row
    row_count++;

      // try to determine separator
    if (!sep_fixed) {
      if (line.contains(',')) { 
	if (sep_seen && sep!=',') is_consistent = false;
	else { sep=','; sep_seen = true; }
      }
      if (line.contains('\t')) { 
	if (sep_seen && sep!='\t') is_consistent = false;
	else { sep='\t'; sep_seen = true; }
      }
    }

      // get how many columns in this line
    int c = line.contains(sep) + 1;
    if (verbose) printf("line: %d cols = %d\n", curr_line, c);
      // if this is different from a previous row then note
    if (max_cols != 0 && c != max_cols) is_consistent = false;
      // maximum number of columns in any line
    if (c > max_cols) max_cols = c;
    //jlog::ls << "curr_lime " << curr_line << " " << c << "\n";
  }  

  f.close();

  return true;
}



// JTextTable::(constructor)

JTextTable::JTextTable() : 
  cols(0),
  str_list(0),
  cols_fixed(false),
  sep(','),
  sep_fixed(false),
  output_sep("\t"),
  double_array(0),
  text_array(0) { }



// JTextTable::(constructor)

JTextTable::JTextTable(int c) :
  cols_fixed(true),
  sep(','),
  sep_fixed(false),
  output_sep("\t"),
  double_array(0),
  text_array(0) {

  allocate(c);
}



// JTextTable::(destructor)

JTextTable::~JTextTable() {
  clear();
}



// JTextTable::allocate(int c) [private]

void JTextTable::allocate(int c) {
  cols = c;
  str_list = new JList<JString>[c];
  for (int i=0;i<cols;i++) str_list[i].setAutoDelete(true);
}



// JTextTable::setColumnCount()

void JTextTable::setColumnCount(int c) {
  cols_fixed = true;
  allocate(c);
}



// JTextTable::setSeperator()

void JTextTable::setSeperator(JChar s) {
  sep = s;
  sep_fixed = true;
}



// JTextTable::clear()

void JTextTable::clear() {
  delete[] str_list;
  str_list = 0;
  cols = 0;
  delete[] double_array;
  double_array = 0;
  delete[] text_array;
  text_array = 0;
}



// JTextTable::getText()

const JString& JTextTable::getText(int r, int c) const {

  if (c >= cols) return JString::null;
  if (r >= str_list[c].count()) return JString::null;

  return *str_list[c].at(r);
}



// JTextTable::setText()

bool JTextTable::setText(int r, int c, const JString& s) {

  if (c >= cols) { printf("JTextTable::setText() column out of range"); return false; }

  while (r >= str_list[c].count()) {
    for (int i=0;i<cols;i++) str_list[i].append(new JString(""));
  }

  *str_list[c].at(r) = s;

  return true;
}



// JTextTable::vectorFromColumn()

void JTextTable::vectorOfDoublesFromColumn(std::vector<double>& v, int col) {
  v.resize(rowCount());
  for (int r=0; r<rowCount(); r++) v[r] = getText(r, col).toDouble();
}



//void JTextTable::vectorOfDoublesFromColumn(vector<double>& v, int col) {
//  v.resize(rowCount());
//  for (int r=0; r<rowCount(); r++) v(r) = getText(r, col).toDouble();
//}



// JTextTable::vectorOfDoublesFromColumn()
// r1 inclusive r2 exclusive

//void JTextTable::vectorOfDoublesFromColumn(vector<double>& v, int col, int r1, int r2) {
//  v.resize(r2 - r1);
//  for (int r=r1; r<r2; r++) v(r-r1) = getText(r, col).toDouble();
//}



// JTextTable::readFromFile()

bool JTextTable::readFromFile(const JString& filepath, bool make_doubles, int double_row_count) {

  bool sep_seen = false;

  if (make_doubles && double_row_count>0) {
    delete[] double_array;
    double_array = new double[double_row_count*cols];
    delete[] text_array;
    text_array = new JString[double_row_count*cols];
  }

    // open the file
  JFile f(filepath);
  if (!f.open(JIO_ReadOnly)) return false;
  
    // get a text stream on the file
  JTextStream str(&f);

  JString line;
  int curr_line = 0;
  int curr_row = 0;

  while (!f.atEnd()) {
    
      // read next line ignoring comments and empty lines
    do { line = str.readLine(); curr_line++; } 
    while ((line.isEmpty() || line.stripWhiteSpace().left(1)=="#") && !f.atEnd());

    //jlog::ls << "line " << curr_line << " len " << line.length() << " " << line.contains(sep) << " " << (int) sep << "\n";
    //if (curr_line == 3320) {
    // for (int i=0; i< line.length(); i++) jlog::ls << " " << (int) line.at(i);
    //  jlog::ls << "\n"; 
    // }

    if (f.atEnd()) break;

    if (cols==0) allocate(line.contains(',') + 1);

      // try to determine separator
    if (!sep_seen && !sep_fixed) {
      if (line.contains(',')) { sep=','; sep_seen = true; }  
      else if (line.contains('\t')) { sep='\t'; sep_seen = true; }
    }

    if (!cols_fixed && line.contains(sep) != cols-1) { 
      jlog::es.printf("Syntax error in file '%s' line %d (wrong number of seperators on a line %d should be %d)\n",filepath.latin1(),curr_line,line.contains(sep),cols-1);
      return false;
    }
    //jlog::ls << "curr_lime " << curr_line << " " << line.contains(sep) << "\n";

      // exceeds number of columns
    if (line.contains(sep)+1 > cols) return false;

    int i = 0; 
    for (i=0; i<=line.contains(sep); i++) {
      JString* s = new JString(line.section(sep, i, i).stripWhiteSpace());
      str_list[i].append(s);
      if (make_doubles && curr_row<double_row_count) {
	double v = s->toDouble();
	double_array[curr_row*cols+i] = v;
	if (v==0 && !s->isEmpty()) text_array[curr_row*cols+i] = *s;
      }
    }
    for (; i<cols; i++) str_list[i].append(new JString(""));
    curr_row++;
  }  

  f.close();

  return true;
}



// JTextTable::writeToFile()

bool JTextTable::writeToFile(const JString& filepath) const {

    // open the file
  JFile f(filepath);
  if (!f.open(JIO_WriteOnly)) { printf("Couldn't open file to write '%s'",filepath.latin1()); return false; }
  
    // get a text stream on the file
  JTextStream str(&f);       

  for (int i=0; i<rowCount(); i++) {
    for (int j=0; j<columnCount(); j++) {
      str << getText(i,j).latin1();
      if (j < columnCount()-1) str << output_sep;
    }
    str << "\n";
  }

  f.close();

  return true;
}



// JTextTable::display()

void JTextTable::display() const {

  printf("%d rows, %d columns\n", rowCount(), columnCount());

  for (int i=0; i<rowCount(); i++) {
    for (int j=0; j<columnCount(); j++) {
      printf("%s",getText(i,j).latin1());
      if (j < columnCount()-1) printf(", ");
    }
    printf("\n");
  }
}

