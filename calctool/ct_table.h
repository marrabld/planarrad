
// ct_table.h

#ifndef _JUDETOOLS_CT_TABLE_H
#define _JUDETOOLS_CT_TABLE_H

#include "base/jstring.h"
#include "base/jtexttable.h"

using namespace jude::base;



class CT_Table {

  JString id_str;

  mutable JTextTable* ttab;

  double* table_data;
  int row_count;
  int col_count;

  JString output_sep;
  int output_prec;  

public:
  
  CT_Table(const JString& n);
  ~CT_Table();

  void reallocate(int rn, int cn);

  const JString& id() { return id_str; }

  int rowCount() const { return row_count; }
  int columnCount() const { return col_count; }
  // depreceated
  int colCount() const { return col_count; }

  double* data() { return table_data; }
  const double* data() const { return table_data; }

  bool getSingleValue(double& v) const;
  void setSingleValue(double v); 

  void setOutputScientific(int prec);
  void setOutputSeparator(const JString& s) { output_sep = s; }

  void load(const JString& filepath);
  void save(const JString& filepath) const;
};



#endif // _JUDETOOLS_CT_TABLE_H

