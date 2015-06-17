
#ifndef _PLANARRAD_DATASET_H
#define _PLANARRAD_DATASET_H

#include "specset.h"
#include "base/jstring.h"
#include "base/jtexttable.h"
#include "rad/bandspec.h"
#include "rad/sbanddata.h"
#include "rad/raddirecimp.h"
#include "raddirecset.h"
#include "ds/spherepartvxh.h"

using namespace jude::base;




class DataSet {

  JString fp;

  JString name_str;

  //BandSpec* bs;

  SpherePartVxH* ds;

  double max_depth;

  bool is_model;

  bool is_ok;

  bool readFromFile(const JString& fp);

  SpecSet Ed_set;
  SpecSet Eu_set;

  SpecSet Ld_set;
  SpecSet Lu_set;

  SpecSet Eo_set;
  SpecSet Eod_set;
  SpecSet Eou_set;

  SpecSet Kd_set;
  SpecSet Ku_set;

  SpecSet R_set;
  SpecSet Rtot_set;

  RadDirecSet L_direc_set;

  double min_waveln;
  double max_waveln;

  void createDirecStructFromReport(JTextTable& ttab);
  void readRadianceTable(const JString& code, const JTextTable& ttab, int& curr_row);

  void readSpecSetFromDataFile(SpecSet& spec_set, const JString& code, const JTextTable& ttab, int depth_col, bool& has_data);

public:

  DataSet(const JString& fp);

  const JString& filePath() const { return fp; }

  const JString& name() const { return name_str; }

  bool isModel() const { return is_model; }

  const SpecSet& EdSet() const { return Ed_set; }
  const SpecSet& EuSet() const { return Eu_set; }

  const SpecSet& LdSet() const { return Ld_set; }
  const SpecSet& LuSet() const { return Lu_set; }

  const SpecSet& EoSet() const { return Eo_set; }
  const SpecSet& EodSet() const { return Eod_set; }
  const SpecSet& EouSet() const { return Eou_set; }

  const SpecSet& KdSet() const { return Kd_set; }
  const SpecSet& KuSet() const { return Ku_set; }

  const SpecSet& RSet() const { return R_set; }
  const SpecSet& RtotSet() const { return Rtot_set; }
 
  const RadDirecSet& LDirecSet() const { return L_direc_set; }

  double maxDepth() const { return max_depth; }
  
  double minWavelength() const { return min_waveln; }
  double maxWavelength() const { return max_waveln; }

  //double minWavelength() const { return bs->bandCentres()(0); }
  //double maxWavelength() const { return bs->bandCentres()(bs->bandCount()-1); }

  bool isOk() const { return is_ok; }

  bool readFromDataFile(const JTextTable& ttab);

};



#endif // _PLANARRAD_DATALSET_H

