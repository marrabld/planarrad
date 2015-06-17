
// calctool.cc

#include "calctool.h"
#include "base/jfile.h"
#include "base/jlog.h"


// CalcTool::(constructor)

CalcTool::CalcTool() :
  Tool(),
  output_sep(""),
  output_prec(4) {

  table_list.setAutoDelete(true);
}



// CalcTool::(destructor)

CalcTool::~CalcTool() {
  // nothing to do
}



// CalcTool::findTable()
// find a table by its id returns null if doesn't exist

CT_Table* CalcTool::findTable(const JString& tab_id) const {
  CT_Table* tab = table_list.first();
  while (tab) {
    if (tab->id() == tab_id) return tab;
    tab = table_list.next();
  }
  return tab; 
}



// CalcTool::findOrAddTable()
// find a table by its id - creates null table if it doesn't exist

CT_Table* CalcTool::findOrAddTable(const JString& tab_id) {
  CT_Table* tab = findTable(tab_id);
  if (tab) return tab;
  tab = new CT_Table(tab_id);
  table_list.append(tab);
  return tab;
}



// CalcTool::getSingleValue()

bool CalcTool::getSingleValue(const JString& s, double& v) {
    // test if the string is a table name
  CT_Table* tab = findTable(s);
  if (tab) return tab->getSingleValue(v);
    // otherwise just assume is a double value
  v = s.toDouble();
  return true;
}



// CalcTool::run()

bool CalcTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
  Tool::initRandom(prm);

    // check a script is specified
  if (!prm.isDefined("script_fp")) {
    jlog::ls << "A script to execute must be specified using script_fp=<filepath>\n";
    return false;
  }
    // file name for script
  JString filepath = prm.getString("script_fp");;
    // open the file
  JFile f(filepath);
  if (!f.open(JIO_ReadOnly)) { jlog::ls.printf("Couldn't open script file '%s'",filepath.latin1()); return false; }
    // get a text stream on the file
  JTextStream str(&f);       

  if (prm.isDefined("output_separator")) output_sep = prm.getString("output_separator");
  if (prm.isDefined("output_prec")) output_prec = prm.getInt("output_prec");

  JString line;
  int curr_line = 0;

  JString sub[10];

  while (!f.atEnd()) {
    
      // read next line ignoring comments and empty lines
    do { line = str.readLine(); curr_line++; } 
    while ((line.isEmpty() || line.stripWhiteSpace().left(1)=="#") && !f.atEnd());

      // quit if got to the end of the script file
    if (f.atEnd()) break;

    if (jlog::vbLevel(3)) jlog::ls.printf("line '%s'\n",line.latin1());

    // syntax of a line is
    // <command>


    int n_subs = line.contains(' ') + 1;
    if (n_subs > 10) { jlog::es << "command with too many parameters on a line"; return false; }
     
    for (int pos=0; pos<n_subs; pos++) sub[pos] = line.section(' ', pos, pos);
	
    if (jlog::vbLevel(3)) {
      for (int pos=0; pos<n_subs; pos++) jlog::ls.printf("%d '%s'\n",pos,sub[pos].latin1());
    }

    if (sub[0] == "load") loadTable(sub[1], sub[2]);
    else if (sub[0] == "save") saveTable(sub[1], sub[2]);
    else if (sub[0] == "copy") copy(sub[1], sub[2]);
    else if (sub[0] == "transpose") transpose(sub[1], sub[2]);
    else if (sub[0] == "sort") sort(sub[1], sub[2], sub[3]);

    else if (sub[0] == "column_subset") columnSubset(sub[1], sub[2], sub[3], sub[4]);
    else if (sub[0] == "column_product") columnProduct(sub[1], sub[2], sub[3]);
    else if (sub[0] == "column_append") columnAppend(sub[1], sub[2], sub[3]);

    else if (sub[0] == "column_sum") columnSum(sub[1], sub[2]);
    else if (sub[0] == "column_mean") columnMean(sub[1], sub[2]);
    else if (sub[0] == "column_mean_and_stdev") columnMeanAndStdev(sub[1], sub[2]);
    else if (sub[0] == "column_block_mean") columnBlockMean(sub[1], sub[2], sub[3], sub[4]);
    else if (sub[0] == "column_bin_sum") columnBinSum(sub[1], sub[2], sub[3], sub[4], sub[5]);
    else if (sub[0] == "column_weighted_histogram") columnWeightedHistogram(sub[1], sub[2], sub[3], sub[4], sub[5], sub[6]);
    else if (sub[0] == "repeated_row_extract") repeatedRowExtract(sub[1], sub[2], sub[3], sub[4]);
 
    else if (sub[0] == "row_subset") rowSubset(sub[1], sub[2], sub[3], sub[4]);
 
    else if (sub[0] == "row_sum") rowSum(sub[1], sub[2]);
    else if (sub[0] == "row_mean") rowMean(sub[1], sub[2]);
    else if (sub[0] == "row_product") rowProduct(sub[1], sub[2], sub[3]);
  
    else if (sub[0] == "add_value") addValue(sub[1], sub[2]);
    else if (sub[0] == "subtract_value") subtractValue(sub[1], sub[2]);
    else if (sub[0] == "multiply_by_value") multiplyByValue(sub[1], sub[2]);
    else if (sub[0] == "divide_by_value") divideByValue(sub[1], sub[2]);

    else if (sub[0] == "negate") negate(sub[1]);
    else if (sub[0] == "make_exp") makeExp(sub[1]);
    else if (sub[0] == "make_ln") makeLn(sub[1]);
    else if (sub[0] == "make_single_value") makeSingleValue(sub[1], sub[2]);

    else if (sub[0] == "get_column_max") getColumnMax(sub[1], sub[2], sub[3], sub[4]);
    else if (sub[0] == "regress_columns") regressColumns(sub[1], sub[2], sub[3], sub[4], sub[5], sub[6], sub[7]);

    else if (sub[0] == "add") add(sub[1], sub[2]);
    else if (sub[0] == "divide") divide(sub[1], sub[2]);

    else if (sub[0] == "fabs") fabsConvert(sub[1]);
    else if (sub[0] == "count_greater_than") countGreaterThan(sub[1], sub[2], sub[3]);
    else if (sub[0] == "sum_greater_than") sumGreaterThan(sub[1], sub[2], sub[3], sub[4]);

    else if (sub[0] == "exit") { 
      if (jlog::vbLevel(2)) jlog::ls << "exit\n";
      break;
    }  
  }

  f.close();

  return true;
}


 
// void CalcTool::loadTable()

// syntax:
// load <filepath> <table_id>

bool CalcTool::loadTable(const JString& filepath, const JString& table_id) {

  if (jlog::vbLevel(2)) jlog::ls << "load_table\n";

  CT_Table* tab = findOrAddTable(table_id);
  tab->load(filepath);
 
  if (jlog::vbLevel(2)) jlog::ls << "Done. (size " << tab->rowCount() << " rows " <<  tab->columnCount() << " columns)\n";

  return true;
}


 
// void CalcTool::saveTable()

bool CalcTool::saveTable(const JString& filepath, const JString& table_id) const {

  if (jlog::vbLevel(2)) jlog::ls << "save_table\n";

  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

  if (!output_sep.isEmpty()) tab->setOutputSeparator(output_sep);
  tab->setOutputScientific(output_prec);

  tab->save(filepath);

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::copy() [private]

bool CalcTool::copy(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "copy\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(rnum, cnum);
  
  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r*cnum+c] = src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::transpose() [private]

bool CalcTool::transpose(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "transpose\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(cnum, rnum);
  
  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[c*rnum+r] = src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::sort() [private]

bool CalcTool::sort(const JString& table_src, const JString& table_targ, const JString& col) {

  if (jlog::vbLevel(2)) jlog::ls << "sort\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

    // column by which to sort in internal array index form 
  double v;
  if (!getSingleValue(col, v)) return false;
  int csort = (int) v - 1;
    // check ok
  if (csort < 0 || csort >= cnum) { 
    jlog::es << "sort column " << csort+1 << " out of range (1-" << cnum << ")\n"; 
    return false; 
  }

  targ_tab->reallocate(rnum, cnum);

  JList<CT_SortElem> sort_list;

    // build a list of sortable references to table rows
  for (int r=0; r<rnum; r++) sort_list.append(new CT_SortElem(src_tab->data()[r*cnum+csort], r));
    // sort the list
  sort_list.sort();

    // construct the new table with rows in sorted order
  CT_SortElem* se = sort_list.first();
  
  for (int r=0; r<rnum; r++) {

    if (!se) { jlog::es << "internal error with sorted list\n"; return false; }

    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r*cnum+c] = src_tab->data()[se->index()*cnum+c];
    }
    se = sort_list.next();
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnSubset() [private]

bool CalcTool::columnSubset(const JString& table_src, const JString& table_targ, const JString& col_start_str, const JString& col_end_str) {

  if (jlog::vbLevel(2)) jlog::ls << "column_subset\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);
 
  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  double v;
  if (!getSingleValue(col_start_str, v)) return false;
  int col_start = (int) v;
  if (!getSingleValue(col_end_str, v)) return false;
  int col_end = (int) v;
  
  if (cnum < col_start || cnum < col_end) { jlog::es << "not enough columns in source"; return false; }

  targ_tab->reallocate(rnum, col_end-col_start+1);

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=col_start-1; c<col_end; c++,i++) {
      targ_tab->data()[i] = src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnAppend() [private]

bool CalcTool::columnAppend(const JString& table_src1, const JString& table_src2, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "column_append\n";

  CT_Table* src1_tab = findTable(table_src1);
  CT_Table* src2_tab = findTable(table_src2);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src1_tab) { jlog::es << "table '" << table_src1 << "' not found\n"; return false; }
  if (!src2_tab) { jlog::es << "table '" << table_src2 << "' not found\n"; return false; }

  int rnum1 = src1_tab->rowCount();
  int rnum2 = src2_tab->rowCount();

  if (rnum1 != rnum2) {
    jlog::es << "row numbers not equal ("<< rnum1 << " vs. "<< rnum2 << ")\n";
    return false;
  }

  int cnum1 = src1_tab->colCount();
  int cnum2 = src2_tab->colCount();

  targ_tab->reallocate(rnum1, cnum1+cnum2);

  for (int i=0,r=0; r<rnum1; r++) {
    for (int c=0; c<cnum1; c++,i++) targ_tab->data()[i] = src1_tab->data()[r*cnum1+c];
    for (int c=0; c<cnum2; c++,i++) targ_tab->data()[i] = src2_tab->data()[r*cnum2+c];
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnSum() [private]

bool CalcTool::columnSum(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "column_sum\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(1, cnum);

  for (int c=0; c<cnum; c++) targ_tab->data()[c] = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[c] += src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnMean() [private]

bool CalcTool::columnMean(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "column_mean\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(1, cnum);

  for (int c=0; c<cnum; c++) targ_tab->data()[c] = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[c] += src_tab->data()[r*cnum+c];
    }
  }

  for (int c=0; c<cnum; c++) targ_tab->data()[c] /= rnum;

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnMean() [private]

bool CalcTool::columnMeanAndStdev(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "column_mean_and_stdev\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(1, 2 * cnum);

  for (int c=0; c<cnum; c++) targ_tab->data()[c] = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[c] += src_tab->data()[r*cnum+c];
    }
  }

  for (int c=0; c<cnum; c++) targ_tab->data()[c] /= rnum;


  for (int c=0; c<cnum; c++) {
    double sum_sq = 0;
    for (int r=0; r<rnum; r++) {
      double d = src_tab->data()[r*cnum+c] - targ_tab->data()[c];
      sum_sq += d*d;
    }
    targ_tab->data()[c + cnum] = sqrt(sum_sq/(rnum-1));
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnBlockMean() [private]

bool CalcTool::columnBlockMean(const JString& table_src, const JString& table_targ, const JString& block_size, const JString& row_start) {

  if (jlog::vbLevel(2)) jlog::ls << "column_block_mean\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

    // number of rows in a block 
  double v;
  if (!getSingleValue(block_size, v)) return false;
  int sz = (int) v;
    // row to start on in internal index array form
  if (!getSingleValue(row_start, v)) return false;
  int roff = (int) v - 1;
    // number of blocks - will discard any extra rows
  int bnum = (rnum - roff) / sz;
 
  //printf("cnum %d bnum %d sz %d\n",cnum,bnum,sz);

  targ_tab->reallocate(bnum, cnum);

    // initialise output to zero
  for (int b=0; b<bnum; b++) {
    for (int c=0; c<cnum; c++) targ_tab->data()[b*cnum+c] = 0;
  }

  for (int b=0; b<bnum; b++) {
    for (int r=b*sz+roff; r<(b+1)*sz+roff; r++) {
      for (int c=0; c<cnum; c++) {
	targ_tab->data()[b*cnum+c] += src_tab->data()[r*cnum+c];
      }
    }
  }

    // make into mean
  for (int b=0; b<bnum; b++) {
    for (int c=0; c<cnum; c++) targ_tab->data()[b*cnum+c] /= sz;
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::columnBinSum() [private]

bool CalcTool::columnBinSum(const JString& table_src, const JString& table_targ, const JString& src_col, const JString& bin_size, const JString& bin_count) {

  if (jlog::vbLevel(2)) jlog::ls << "column_bin_sum\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

    // size of a bin
  double b_size;
  if (!getSingleValue(bin_size, b_size)) return false;
    // number of bins
  double v;
  if (!getSingleValue(bin_count, v)) return false;
  int b_count = (int) v;
    // column to read bin values
  if (!getSingleValue(src_col, v)) return false;
  int b_col = (int) v;
 
  //printf("cnum %d bnum %d sz %d\n",cnum,bnum,sz);

     // table for the results
  targ_tab->reallocate(b_count, cnum+1);

    // initialise to zero
  for (int b=0; b<b_count; b++) {
    for (int c=0; c<cnum+1; c++) targ_tab->data()[b*cnum+c] = 0;
  }

    // for each row of source table
  for (int r=0; r<rnum; r++) {
    v = src_tab->data()[r*cnum + b_col-1];
    int i = (int) floor(v / b_size);
    if (i>=0 && i<b_count) {
      for (int c=0; c<cnum; c++) targ_tab->data()[i*(cnum+1)+c] += src_tab->data()[r*cnum+c]; 
      targ_tab->data()[i*(cnum+1)+cnum]++;
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::columnBinSum() [private]

bool CalcTool::columnWeightedHistogram(const JString& table_src, const JString& table_targ, const JString& src_col, const JString& weight_col, const JString& bin_size, const JString& bin_count) {

  if (jlog::vbLevel(2)) jlog::ls << "column_weighted_histogram\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

    // size of a bin
  double b_size;
  if (!getSingleValue(bin_size, b_size)) return false;
    // number of bins
  double v;
  if (!getSingleValue(bin_count, v)) return false;
  int b_count = (int) v;
    // column to read bin values
  if (!getSingleValue(src_col, v)) return false;
  int b_col = (int) v;
    // column to read weight values
  if (!getSingleValue(weight_col, v)) return false;
  int w_col = (int) v;
 
  //printf("cnum %d bnum %d sz %d\n",cnum,bnum,sz);

     // table for the results
  targ_tab->reallocate(b_count*2, 2);

    // initialise to zero
  for (int b=0; b<b_count; b++) {
    for (int c=0; c<cnum+1; c++) targ_tab->data()[b*4] = b * b_size;
    for (int c=0; c<cnum+1; c++) targ_tab->data()[b*4+2] = (b+1) * b_size;
    for (int c=0; c<cnum+1; c++) targ_tab->data()[b*4+1] = 0;
    for (int c=0; c<cnum+1; c++) targ_tab->data()[b*4+3] = 0;
  }

    // for each row of source table
  for (int r=0; r<rnum; r++) {
    v = src_tab->data()[r*cnum + b_col-1];
    int i = (int) floor(v / b_size);
    if (i>=0 && i<b_count) {
      targ_tab->data()[i*4+1] += src_tab->data()[r*cnum + w_col-1]; 
      targ_tab->data()[i*4+3] += src_tab->data()[r*cnum + w_col-1]; 
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::repeatedRowExtract() [private]

bool CalcTool::repeatedRowExtract(const JString& table_src, const JString& table_targ, const JString& row_pos, const JString& block_size) {

  if (jlog::vbLevel(2)) jlog::ls << "repeated_row_extract\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

    // number of rows in a block 
  double v;
  if (!getSingleValue(block_size, v)) return false;
  int sz = (int) v;
    // number of blocks - will discard any extra rows
  int bnum = rnum / sz;

    // position of row in a block to extract 
  if (!getSingleValue(row_pos, v)) return false;
  int rpos = (int) v;

  //printf("cnum %d bnum %d sz %d\n",cnum,bnum,sz);

  targ_tab->reallocate(bnum, cnum);

  for (int b=0; b<bnum; b++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[b*cnum+c] = src_tab->data()[(b*sz+rpos)*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::rowSubset() [private]

bool CalcTool::rowSubset(const JString& table_src, const JString& table_targ, const JString& row_start, const JString& row_end) {

  if (jlog::vbLevel(2)) jlog::ls << "row_subset\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);
 
  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

    // row limits
  double v;
  if (!getSingleValue(row_start, v)) return false;
  int rstart = ((int) v) - 1;
  if (!getSingleValue(row_end, v)) return false;
  int rend = ((int) v) - 1;
  
  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  if (rstart < 0 || rstart >= rnum) { 
    jlog::es << "start row " << rstart+1 << " out of range (1-" << rnum << ")\n"; 
    return false; 
  }

  if (rend < 0 || rend >= rnum) { 
    jlog::es << "end row " << rend+1 << " out of range (1-" << rnum << ")\n"; 
    return false; 
  }

  targ_tab->reallocate(rend-rstart+1, cnum);

  for (int r=rstart; r<=rend; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[(r-rstart)*cnum+c] = src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::rowSum() [private]

bool CalcTool::rowSum(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "row_sum\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(rnum, 1);

  for (int r=0; r<rnum; r++) targ_tab->data()[r] = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r] += src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::rowMean() [private]

bool CalcTool::rowMean(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "row_mean\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  targ_tab->reallocate(rnum, 1);

  for (int r=0; r<rnum; r++) targ_tab->data()[r] = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r] += src_tab->data()[r*cnum+c];
    }
    targ_tab->data()[r] /= cnum;
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::rowProduct() [private]
// can use mult src as (1 x cnum) or (cnum x 1) table format

bool CalcTool::rowProduct(const JString& table_src, const JString& table_targ, const JString& mult_src) {

  if (jlog::vbLevel(2)) jlog::ls << "row_product\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* mult_tab = findTable(mult_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }
  if (!mult_tab) { jlog::es << "table '" << mult_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  int mult_rnum = mult_tab->rowCount();
  int mult_cnum = mult_tab->colCount();
  
  if (!((mult_cnum==cnum && mult_rnum==1) || (mult_cnum==1 && mult_rnum==cnum))) {
    jlog::es << "table '" << table_src << "' (" << rnum << " x " << cnum << ") and ";
    jlog::es << "'" << mult_src << "' (" << mult_rnum << " x " << mult_cnum << ") incompatible sizes\n";
    return false;
  }

  if (jlog::vbLevel(3)) {
    jlog::es << "table '" << table_src << "' (" << rnum << " x " << cnum << ")\n";
    jlog::es << "table '" << mult_src << "' (" << mult_rnum << " x " << mult_cnum << ")\n";
  }

  targ_tab->reallocate(rnum, cnum);

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r*cnum+c] = src_tab->data()[r*cnum+c] * mult_tab->data()[c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::columnProduct() [private]
// can use mult_src as (1 x rnum) or (rnum x 1) table format

bool CalcTool::columnProduct(const JString& table_src, const JString& table_targ, const JString& mult_src) {

  if (jlog::vbLevel(2)) jlog::ls << "column_product\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* mult_tab = findTable(mult_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }
  if (!mult_tab) { jlog::es << "table '" << mult_src << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  int mult_rnum = mult_tab->rowCount();
  int mult_cnum = mult_tab->colCount();
  
  if (!((mult_cnum==rnum && mult_rnum==1) || (mult_cnum==1 && mult_rnum==rnum))) {
    jlog::es << "table '" << table_src << "' (" << rnum << " x " << cnum << ") and ";
    jlog::es << "'" << mult_src << "' (" << mult_rnum << " x " << mult_cnum << ") incompatible sizes\n";
    return false;
  }

  if (jlog::vbLevel(3)) {
    jlog::es << "table '" << table_src << "' (" << rnum << " x " << cnum << ")\n";
    jlog::es << "table '" << mult_src << "' (" << mult_rnum << " x " << mult_cnum << ")\n";
  }

  targ_tab->reallocate(rnum, cnum);

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r*cnum+c] = src_tab->data()[r*cnum+c] * mult_tab->data()[r];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::multiplyByValue() [private]

bool CalcTool::multiplyByValue(const JString& table_id, const JString& value) {

  if (jlog::vbLevel(2)) jlog::ls << "multiply_by_value\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

  double v;
  if (!getSingleValue(value, v)) return false;
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] *= v;
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::divideByValue() [private]

bool CalcTool::divideByValue(const JString& table_id, const JString& value) {

  if (jlog::vbLevel(2)) jlog::ls << "divide_by_value\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

  double v;
  if (!getSingleValue(value, v)) return false;

  if (jlog::vbLevel(2)) jlog::ls.printf("divisor = %f\n",v);

  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] /= v;
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::addValue() [private]

bool CalcTool::addValue(const JString& table_id, const JString& value) {

  if (jlog::vbLevel(2)) jlog::ls << "add_value\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

  double v;
  if (!getSingleValue(value, v)) return false;
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] += v;
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::subtractValue() [private]

bool CalcTool::subtractValue(const JString& table_id, const JString& value) {

  if (jlog::vbLevel(2)) jlog::ls << "subtract_value\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

  double v;
  if (!getSingleValue(value, v)) return false;
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] -= v;
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::negate() [private]

// syntax:
// negate <table_id>

bool CalcTool::negate(const JString& table_id) {

  if (jlog::vbLevel(2)) jlog::ls << "negate\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  //jlog::ls << "size: " << rnum << " x " << cnum << "\n";
  //jlog::ls.printf("%p",tab);1

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] = -tab->data()[i];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::makeExp() [private]

bool CalcTool::makeExp(const JString& table_id) {

  if (jlog::vbLevel(2)) jlog::ls << "make_exp\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] = exp(tab->data()[i]);
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::makeLn() [private]

bool CalcTool::makeLn(const JString& table_id) {

  if (jlog::vbLevel(2)) jlog::ls << "make_ln\n";
 
  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  for (int i=0,r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++, i++) {
      tab->data()[i] = log(tab->data()[i]);
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::makeSingleValue() [private]

bool CalcTool::makeSingleValue(const JString& table_id, const JString& value) {

  if (jlog::vbLevel(2)) jlog::ls << "make_single_value\n";
 
  CT_Table* tab = findOrAddTable(table_id);
  
  double v;
  if (!getSingleValue(value, v)) return false;

  tab->reallocate(1,1);
  tab->data()[0] = v;
 
  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::getColumnMax() [private]

bool CalcTool::getColumnMax(const JString& table_id, const JString& col, const JString& val_targ, const JString& row_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "get_column_max\n";

  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

    // column to find max value of
  double v;
  if (!getSingleValue(col, v)) return false;
  int c = ((int) v) - 1;
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  double max_val = -HUGE;
  int max_row = -1;

  for (int r=0; r<rnum; r++) {
    v = tab->data()[r*cnum+c];
    if (v >= max_val) { 
      max_val = v; 
      max_row = r; 
    }
  }

  CT_Table* val_tab = findOrAddTable(val_targ);
  CT_Table* row_tab = findOrAddTable(row_targ);

  val_tab->setSingleValue(max_val);
  row_tab->setSingleValue(max_row + 1);

  if (jlog::vbLevel(2)) jlog::ls.printf("max value = %f row = %d\n", max_val, max_row+1);

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::regressColumns()

bool CalcTool::regressColumns(const JString& table_id, const JString& col_x, const JString& col_y, const JString& row_start, const JString& row_end, const JString& slope_targ, const JString& int_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "regress_columns\n";

  CT_Table* tab = findTable(table_id);
  if (!tab) { jlog::es << "table '" << table_id << "' not found\n"; return false; }

    // columns to use
  double v;
  if (!getSingleValue(col_x, v)) return false;
  int cx = ((int) v) - 1;
  if (!getSingleValue(col_y, v)) return false;
  int cy = ((int) v) - 1;

    // row limits
  if (!getSingleValue(row_start, v)) return false;
  int rstart = ((int) v) - 1;
  if (!getSingleValue(row_end, v)) return false;
  int rend = ((int) v) - 1;
  
  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  if (rstart < 0 || rstart >= rnum) { 
    jlog::es << "start row " << rstart+1 << "out of range (1-" << rnum << ")\n"; 
    return false; 
  }

  if (rend < 0 || rend >= rnum) { 
    jlog::es << "start row " << rend+1 << "out of range (1-" << rnum << ")\n"; 
    return false; 
  }

  double sx = 0;
  double sy = 0;
  double sxy = 0;
  double sxx = 0;
  double syy = 0;
  int n = rend - rstart + 1;

  for (int r=rstart; r<=rend; r++) {
  
      // x and y values
    double xd = tab->data()[r*cnum + cx];
    double yd = tab->data()[r*cnum + cy];

    sx += xd;
    sy += yd;
    sxx += xd*xd;
    syy += yd*yd;
    sxy += xd*yd;
  }

    // calculate regression of y on x
  double cxy = (n*sxy - sx*sy);
  double cxx = (n*sxx - sx*sx);
  double cyy = (n*syy - sy*sy);
  
  double m = cxy / cxx;
  double b = (sy - m*sx) / n;
      
  double r_sq = (cxy * cxy) / (cxx * cyy);

  if (jlog::vbLevel(2)) {
    jlog::ls << "n = " << n << "\n";
    jlog::ls << "x mean = " << sx/n << "\n";
    jlog::ls << "y mean = " << sy/n << "\n";
    jlog::ls << "for y = mx + b : m = " << m << " b = " << b << " r_sq = " << r_sq << "\n";
  }

  CT_Table* slope_tab = findOrAddTable(slope_targ);
  CT_Table* int_tab = findOrAddTable(int_targ);

  slope_tab->setSingleValue(m);
  int_tab->setSingleValue(b);

  if (jlog::vbLevel(2)) jlog::ls.printf("slope = %f intercept = %f\n", m, b);

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}


 
// CalcTool::add() [private]

// syntax:
// add <src_table_id> <targ_table_id>

bool CalcTool::add(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "add\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }
  if (!targ_tab) { jlog::es << "table '" << table_targ << "' not found\n"; return false; }

  int rnum = src_tab->rowCount();
  int cnum = src_tab->colCount();

  if (rnum != targ_tab->rowCount() || cnum != targ_tab->colCount()) {
    jlog::es << "tables '" << table_src << " (" << rnum << " x " << cnum << ") ";
    jlog::es << "and '" << table_targ << " (" << targ_tab->rowCount() << " x " << targ_tab->colCount() << ") not equal sizes\n";
    return false;
  }
  
  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      targ_tab->data()[r*cnum+c] += src_tab->data()[r*cnum+c];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}




// CalcTool::add() [private]
// update a table by doing an element divide with another
// syntax:
// divide <src_table_id> <targ_table_id>

bool CalcTool::divide(const JString& table_src, const JString& table_targ) {

  if (jlog::vbLevel(2)) jlog::ls << "divide\n";

  CT_Table* src_tab = findTable(table_src);
  CT_Table* targ_tab = findOrAddTable(table_targ);

  if (!src_tab) { jlog::es << "table '" << table_src << "' not found\n"; return false; }
  if (!targ_tab) { jlog::es << "table '" << table_targ << "' not found\n"; return false; }

  int rnum = targ_tab->rowCount();
  int cnum = targ_tab->colCount();

  //if (rnum != targ_tab->rowCount() || cnum != targ_tab->colCount()) {
  //  jlog::es << "tables '" << table_src << " (" << rnum << " x " << cnum << ") ";
  //  jlog::es << "and '" << table_targ << " (" << targ_tab->rowCount() << " x " << targ_tab->colCount() << ") not equal sizes\n";
  //  return false;
  //}
  
  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      int r2 = r % src_tab->rowCount();
      int c2 = c % src_tab->colCount();
      if (targ_tab->data()[r*cnum+c]!=0) targ_tab->data()[r*cnum+c] /= src_tab->data()[r2*src_tab->colCount()+c2];
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::fabs() [private]
// update a table by making is the fabs() value
// syntax:
// fabs <table_id>

bool CalcTool::fabsConvert(const JString& table) {

  if (jlog::vbLevel(2)) jlog::ls << "fabs\n";

  CT_Table* tab = findTable(table);

  if (!tab) { jlog::es << "table '" << table << "' not found\n"; return false; }

  int rnum = tab->rowCount();
  int cnum = tab->colCount();
  
  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      tab->data()[r*cnum+c] = fabs(tab->data()[r*cnum+c]); 
    }
  }

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// CalcTool::fabs() [private]
// syntax:
// count_greater_than <src_table_id> <value> <targ_table_id>

bool CalcTool::countGreaterThan(const JString& table, const JString& value, const JString& table_out) {

  if (jlog::vbLevel(2)) jlog::ls << "count_greater_than\n";

  CT_Table* tab = findTable(table);

  if (!tab) { jlog::es << "table '" << table << "' not found\n"; return false; }
  
  double v;
  if (!getSingleValue(value, v)) return false;

  int rnum = tab->rowCount();
  int cnum = tab->colCount();

  int count = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      if (tab->data()[r*cnum+c] > v) count++;
    }
  }

  CT_Table* targ_tab = findOrAddTable(table_out);
  targ_tab->reallocate(1,1);
  targ_tab->data()[0] = count;

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}



// syntax:
// sum_greater_than <compare_value_table_id> <sum_value_table_id> <compare_value> <targ_table_id>

bool CalcTool::sumGreaterThan(const JString& table_val, const JString& table_sum, const JString& value, const JString& table_out) {

  if (jlog::vbLevel(2)) jlog::ls << "sum_greater_than\n";

  CT_Table* tab_val = findTable(table_val);
  if (!tab_val) { jlog::es << "table '" << table_val << "' not found\n"; return false; }

  CT_Table* tab_sum = findTable(table_sum);
  if (!tab_sum) { jlog::es << "table '" << table_sum << "' not found\n"; return false; }
  
  double v;
  if (!getSingleValue(value, v)) return false;

  int rnum = tab_val->rowCount();
  int cnum = tab_val->colCount();

  if (rnum != tab_sum->rowCount() || cnum != tab_sum->colCount()) {
    jlog::es << "tables '" << table_val << " (" << rnum << " x " << cnum << ") ";
    jlog::es << "and '" << table_sum << " (" << tab_sum->rowCount() << " x " << tab_sum->colCount() << ") not equal sizes\n";
    return false;
  }

  double sum = 0;

  for (int r=0; r<rnum; r++) {
    for (int c=0; c<cnum; c++) {
      // printf("%f %f %f \n",tab_val->data()[r*cnum+c],v,sum);
      if (tab_val->data()[r*cnum+c] > v) sum += tab_sum->data()[r*cnum+c];
    }
  }

  CT_Table* targ_tab = findOrAddTable(table_out);
  targ_tab->reallocate(1,1);
  targ_tab->data()[0] = sum;

  if (jlog::vbLevel(2)) jlog::ls << "Done.\n";

  return true;
}





void CalcTool::helpText(JTextStream& str) const {

}

