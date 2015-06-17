
// jtexttable.h

#ifndef _JUDE_TOOL_JTEXTTABLE_H
#define _JUDE_TOOL_JTEXTTABLE_H

#include "base/jlist.h"
#include "base/jstring.h"
#include <vector>

namespace jude {
namespace base {



/*! For reading and writing files as tables of text entries.

\todo This may be better located in a lower down namespace. For consideration. */

class JTextTable {

  int cols;
  JList<JString>* str_list;

  bool cols_fixed;
 
  JChar sep;
  bool sep_fixed;

  JString output_sep;

  double* double_array;
  JString* text_array;

  void allocate(int c);

public:

  static bool getSize(int& row_count, int& max_cols, bool& is_consistent, const JString& filepath, JChar sep=0, bool verbose=false);

  JTextTable();
  JTextTable(int c);
  ~JTextTable();

  void clear();

  void setColumnCount(int c);
  void setSeperator(JChar s);
  void setOutputSeparator(const JString& s) { output_sep = s; }

  int columnCount() const { return cols; }
  int rowCount() const { return (cols==0) ? 0 : str_list[0].count(); }

  const JString& getText(int row, int col) const;
  bool setText(int row, int col, const JString& s);

  double getDoubleFromArray(int row, int col) const { return double_array[row*cols + col]; }
  const JString& getTextFromArray(int row, int col) const { return text_array[row*cols + col]; }

  void vectorOfDoublesFromColumn(std::vector<double>& v, int col);

  //void vectorOfDoublesFromColumn(vector<double>& v, int col);
  //void vectorOfDoublesFromColumn(vector<double>& v, int col, int r1, int r2);
  bool readFromFile(const JString& filepath) { return readFromFile(filepath, false,0); }

  bool readFromFile(const JString& filepath, bool make_doubles, int double_row_count);
  bool writeToFile(const JString& filepath) const;

  void display() const;
};



} } // namespace::base

#endif // _JUDE_TOOL_JTEXTTABLE_H

