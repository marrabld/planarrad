
#include "ct_table.h"
#include "base/jlog.h"

using namespace jude::base;



// CT_Table::(constructor)

CT_Table::CT_Table(const JString& id_str) : 
  id_str(id_str), 
  ttab(0), 
  table_data(0), 
  row_count(0), 
  col_count(0),
  output_sep(""),
  output_prec(4) { }



// CT_Table::(destructor)

CT_Table::~CT_Table() {
  delete ttab;
  delete[] table_data;
}



// CT_Table::reallocate()

void CT_Table::reallocate(int rn, int cn) {

    // clear any existing allocation
  delete[] table_data;
  delete ttab;
  ttab = 0;
    // set size
  row_count = rn;
  col_count = cn;
    // allocate space
  table_data = new double[rn*cn];
}



// CT_Table::getSingleValue()

bool CT_Table::getSingleValue(double& v) const {

  if (row_count!=1 || col_count!=1) { 
    jlog::es << "treating table '" << id_str << "' as a single value but size is " << row_count << " x " << col_count << "\n";
    return false;
  }

  v = table_data[0];
  return true;
}



// CT_Table::setSingleValue()

void CT_Table::setSingleValue(double v) {
  reallocate(1,1);
  table_data[0] = v;
}



void CT_Table::setOutputScientific(int prec) { 
  //output_scientific = true;
  output_prec = prec;
}



// CT_Table::load()

void CT_Table::load(const JString& filepath) {

  int rc;
  int max_cols;
  bool is_consistent;

  JTextTable::getSize(rc, max_cols, is_consistent, filepath);

  if (jlog::vbLevel(2)) {
    jlog::ls << "Loading table '"<< id_str << "' from file '" << filepath << "'\n";
    jlog::ls.printf("TextTable: row_count %d max_cols %d is_consistent %d\n", rc, max_cols, is_consistent); 
  }

  reallocate(rc, max_cols);

  ttab = new JTextTable(max_cols);
  ttab->readFromFile(filepath);

  for (int r=0; r<row_count; r++) {
    for (int c=0; c<col_count; c++) {
      table_data[r*col_count+c] = ttab->getText(r,c).toDouble();
    }
  }

  if (jlog::vbLevel(2)) ttab->display();
}



// CT_Table::save()

void CT_Table::save(const JString& filepath) const {

  if (ttab) {
    delete ttab;
    ttab = 0;
  }

    // build text table if required
  if (!ttab) {

    ttab = new JTextTable(col_count);

    if (!output_sep.isEmpty()) ttab->setOutputSeparator(output_sep);

    for (int r=0; r<row_count; r++) {
      for (int c=0; c<col_count; c++) { 
	ttab->setText(r, c, JString::number(table_data[r*col_count+c], 'e', output_prec));
      }
    }
  }

  ttab->writeToFile(filepath);
}


