
#include "dataset.h"
#include "base/jtexttable.h"
#include "base/jlog.h"
#include "rad/raddirectable.h"



// DataSet::(constructor)

DataSet::DataSet(const JString& fp) : 
  fp(fp),
  ds(0),
  max_depth(0),
  is_model(true),
  is_ok(false) { 

  int slash_count = fp.contains('/');
  if (!slash_count) name_str = JString("(") + fp + ")";
  else name_str = JString("(") + fp.section('/',slash_count,slash_count) + ")";

  is_ok = readFromFile(fp);
}



// DataSet::readFromFile()

bool DataSet::readFromFile(const JString& fp) {

  if (jlog::vb(5)) jlog::ls << "Reading DataSet '" << fp << "'\n";

  max_depth = 0;

  int row_count;
  int max_cols;
  bool is_consistent;

  JTextTable::getSize(row_count, max_cols, is_consistent, fp, ',');

  jlog::ls << "row_count " << row_count << " max_cols " << max_cols << "\n";

  JTextTable ttab;
  ttab.setColumnCount(max_cols);
  if (!ttab.readFromFile(fp, true, row_count)) {
    jlog::es << "Problem reading file - check format";
    return false;
  }

  BandSpec* bs = BandSpec::createFromReport(ttab);
    // if no band spec then try reading from data file
  if (!bs) return readFromDataFile(ttab); 

  min_waveln = bs->bandCentres()(0);
  max_waveln = bs->bandCentres()(bs->bandCount()-1);

  createDirecStructFromReport(ttab);

  Ed_set.setBandSpec(bs);
  Eu_set.setBandSpec(bs);
  Ld_set.setBandSpec(bs);
  Lu_set.setBandSpec(bs);
  Eo_set.setBandSpec(bs);
  Eod_set.setBandSpec(bs);
  Eou_set.setBandSpec(bs);
  Ku_set.setBandSpec(bs);
  Kd_set.setBandSpec(bs);
  R_set.setBandSpec(bs);
  Rtot_set.setBandSpec(bs);

  Ed_set.initValueRange();
  Eu_set.initValueRange();
  Ld_set.initValueRange();
  Lu_set.initValueRange();
  Eo_set.initValueRange();
  Eod_set.initValueRange();
  Eou_set.initValueRange();
  Ku_set.initValueRange();
  Kd_set.initValueRange();
  R_set.initValueRange();
  Rtot_set.initValueRange();

  L_direc_set.setBandSpec(bs);
  L_direc_set.setDirecSpec(ds);

  if (jlog::vb(5)) bs->display(jlog::ls);

  int curr_row = 0;

    // go through rowws of table
  while (curr_row < ttab.rowCount()) {

      // code label for the data on this row or next
    const JString& code = ttab.getTextFromArray(curr_row, 0).lower();

    //jlog::ls << code << "\n";

    if (code == "name") {
      name_str = ttab.getTextFromArray(curr_row, 2);
      curr_row++;
      continue;
    }

    if (code.left(8) == "l_sample") {  
      readRadianceTable(code, ttab, curr_row);
      continue;
    }

      // this will be the set of data to write to
    SpecSet* spec_set = 0;

      // start looking for one-letter codes
    JString sub_code = code.left(2);
      // this will be the length of the sub code to remove
    int spos = 2;  
    if (sub_code == "r_") spec_set = &R_set;
  
      // looking for two-letter codes
    if (!spec_set) {
      sub_code = code.left(3);
        // this will be the length of the sub code to remove
      spos = 3;
      if (sub_code == "ed_") spec_set = &Ed_set;
      else if (sub_code == "eu_") spec_set = &Eu_set;
      else if (sub_code == "ld_") spec_set = &Ld_set;
      else if (sub_code == "lu_") spec_set = &Lu_set;
      else if (sub_code == "eo_") spec_set = &Eo_set;
      else if (sub_code == "ku_") spec_set = &Ku_set;
      else if (sub_code == "kd_") spec_set = &Kd_set;
    }

    if (!spec_set) {
      sub_code = code.left(4);
      spos = 4;
      if (sub_code == "eod_") spec_set = &Eod_set;
      else if (sub_code == "eou_") spec_set = &Eou_set;
    }

    if (!spec_set) {
      sub_code = code.left(5);
      spos = 5;
      if (sub_code == "rtot_") spec_set = &Rtot_set;
    }

    if (!spec_set || code.length() - spos <= 0) { curr_row++; continue; }

    JString type_code(code.right(code.length() - spos));
    
    if (type_code == "a") spec_set->loadAir(ttab, curr_row, 1, bs->bandCount());
    else if (type_code == "w") spec_set->loadWater(ttab, curr_row, 1, bs->bandCount());
    else if (type_code == "b") spec_set->loadBottom(ttab, curr_row, 1, bs->bandCount());
    else if (type_code.left(7) == "sample_" && type_code.length() > 8) {
      JString s = type_code.right(type_code.length() - 7);
      double d = s.left(s.length() - 1).toDouble();
      spec_set->loadSample(d, ttab, curr_row, 2, bs->bandCount());
      if (d > max_depth) max_depth = d;
    }

      // next row  
    curr_row++;
  }

  Ed_set.finaliseValueRange();
  Eu_set.finaliseValueRange();
  Ld_set.finaliseValueRange();
  Lu_set.finaliseValueRange();
  Eo_set.finaliseValueRange();
  Eod_set.finaliseValueRange();
  Eou_set.finaliseValueRange();
  Ku_set.finaliseValueRange();
  Kd_set.finaliseValueRange();
  R_set.finaliseValueRange();
  Rtot_set.finaliseValueRange();

  is_model = true;

  delete bs;

  return true;
}



void DataSet::createDirecStructFromReport(JTextTable& ttab) {

  int vn = -1;
  int hn = -1;

  std::vector<double> tp;

  for (int i=0; i<ttab.rowCount() && (vn<0 || hn<0 || tp.size()==0); i++) {
    if (ttab.getText(i,0) == "vn") vn = ttab.getText(i,2).toInt();
    if (ttab.getText(i,0) == "hn") hn = ttab.getText(i,2).toInt();
    if (ttab.getText(i,0) == "theta_points_deg") {
      tp.resize(vn+3);
      for (int j=0; j<vn+3; j++) tp[j] = ttab.getDoubleFromArray(i,j+2);
    }
  }

  jlog::ls << "vn " << vn << " hn " << hn << " tp size " << (int)(tp.size()) << "\n";

  if (vn < 0 || hn < 0) return;

  ds = new SpherePartVxH(vn, hn, tp);
}



void DataSet::readRadianceTable(const JString& code, const JTextTable& ttab, int& curr_row) {

  jlog::ls << "Read table " << code << "\n";

  JString s1 = code.section(' ', 0, 0);
  JString s2 = code.section(' ', 2, 2);

  s1 = s1.left(s1.length()-1);
  s1 = s1.right(s1.length() - 9);
  double depth = s1.toDouble();

  int band = s2.toInt();

  //jlog::ls << s << "\n";

  jlog::ls << "depth " << depth << " band " << band << "\n";

  RadDirecImp<SBandData>* rd = L_direc_set.findOrCreate(depth);
  if (!rd) return;

  RadDirecQDW<SBandData> qdw(*rd, band-1);
  rd->direcStruct().readTableValues(ttab, curr_row+1, 0, qdw);

  curr_row += ds->vertSize()+1;
}
 


bool DataSet::readFromDataFile(const JTextTable& ttab) {

  int depth_col = 0; 

  for (;depth_col<ttab.columnCount(); depth_col++) {
    JString s = ttab.getTextFromArray(0, depth_col);
    s = s.lower();
    if (s == "depth") break;
  }

  if (depth_col == ttab.columnCount()) {
    jlog::es << "No depth column found.\n";
    return false;
  }

  jlog::ls << "depth column: " << depth_col << "\n";

  Ed_set.initValueRange();
  Eu_set.initValueRange();
  Ld_set.initValueRange();
  Lu_set.initValueRange();
  Eo_set.initValueRange();
  Eod_set.initValueRange();
  Eou_set.initValueRange();
  Ku_set.initValueRange();
  Kd_set.initValueRange();
  R_set.initValueRange();
  Rtot_set.initValueRange();

  min_waveln = 100000;
  max_waveln = -100000;

  bool has_data = false;

  readSpecSetFromDataFile(Ed_set, "ed", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Eu_set, "eu", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Ld_set, "ld", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Lu_set, "lu", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Eo_set, "eo", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Eod_set, "eod", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Eou_set, "eou", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Ku_set, "ku", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Kd_set, "kd", ttab, depth_col, has_data);
  readSpecSetFromDataFile(R_set, "r", ttab, depth_col, has_data);
  readSpecSetFromDataFile(Rtot_set, "rtot", ttab, depth_col, has_data);

  if (!has_data) {
    jlog::es << "File contains no data.\n";
    return false;
  }

  Ed_set.finaliseValueRange();
  Eu_set.finaliseValueRange();
  Ld_set.finaliseValueRange();
  Lu_set.finaliseValueRange();
  Eo_set.finaliseValueRange();
  Eod_set.finaliseValueRange();
  Eou_set.finaliseValueRange();
  Ku_set.finaliseValueRange();
  Kd_set.finaliseValueRange();
  R_set.finaliseValueRange();
  Rtot_set.finaliseValueRange();

  is_model = false;

  return true;
}



void DataSet::readSpecSetFromDataFile(SpecSet& spec_set, const JString& code, const JTextTable& ttab, int depth_col, bool& has_data) {
  spec_set.readFromDataFile(code, ttab, depth_col, min_waveln, max_waveln);
  has_data = has_data || spec_set.hasSampleData();
  if (spec_set.maxDepth() > max_depth) max_depth = spec_set.maxDepth();
}

