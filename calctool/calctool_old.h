
// calctool.h

#ifndef _JUDETOOLS_CALCTOOL_H
#define _JUDETOOLS_CALCTOOL_H

#include "tool/tool.h"
#include "S3/vec3d.h"
#include "rad/sbanddata.h"
#include "ct_table.h"
#include "ct_sortelem.h"

namespace jude { namespace rad { class BandSpec; } }
namespace jude { namespace ds { class DirecSpec; } }

using namespace jude::tool;



class CalcTool : public Tool {
 
  JList<CT_Table> table_list;

  JString output_sep;
  int output_prec;

  CT_Table* getTable(const JString& table_id) const;
 
  CT_Table* findTable(const JString& tab_id) const;
  CT_Table* findOrAddTable(const JString& tab_id);

  bool getSingleValue(const JString& s, double& v);

  bool loadTable(const JString& filepath, const JString& table_id);
  bool saveTable(const JString& filepath, const JString& table_id) const;
  bool copy(const JString& filepath, const JString& table_id);
  bool transpose(const JString& filepath, const JString& table_id);
  bool sort(const JString& filepath, const JString& table_id, const JString& col);

  bool columnSubset(const JString& table_src, const JString& table_targ, const JString& col_start_str, const JString& col_end_str);
  bool columnAppend(const JString& table_src1, const JString& table_src2, const JString& table_targ);

  bool columnSum(const JString& table_src, const JString& table_targ);
  bool columnProduct(const JString& table_src, const JString& table_targ, const JString& mult_src);
  bool columnMean(const JString& table_src, const JString& table_targ);
  bool columnMeanAndStdev(const JString& table_src, const JString& table_targ);
  bool columnBlockMean(const JString& table_src, const JString& table_targ, const JString& block_size, const JString& row_start);
  bool columnBinSum(const JString& table_src, const JString& table_targ, const JString& src_col, const JString& bin_size, const JString& bin_count);

  bool repeatedRowExtract(const JString& table_src, const JString& table_targ, const JString& row_pos, const JString& block_size);

  bool rowSubset(const JString& table_src, const JString& table_targ, const JString& row_start, const JString& row_end);

  bool rowSum(const JString& table_src, const JString& table_targ);
  bool rowMean(const JString& table_src, const JString& table_targ);
  bool rowProduct(const JString& table_src, const JString& table_targ, const JString& mult_src);

  bool multiplyByValue(const JString& table_id, const JString& value);
  bool divideByValue(const JString& table_id, const JString& value);
  bool addValue(const JString& table_id, const JString& value);
  bool subtractValue(const JString& table_id, const JString& value);

  bool negate(const JString& table_id);
  bool makeExp(const JString& table_id);
  bool makeLn(const JString& table_id);
  bool makeSingleValue(const JString& table_id, const JString& value);

  bool getColumnMax(const JString& table_id, const JString& col, const JString& val_var, const JString& row_var);
  bool regressColumns(const JString& table_id, const JString& col_x, const JString& col_y, const JString& row_start, const JString& row_end, const JString& slope_targ, const JString& int_targ);

  bool add(const JString& table_src, const JString& table_targ);
  bool divide(const JString& table_src, const JString& table_targ);
  bool fabsConvert(const JString& table);

  bool countGreaterThan(const JString& table, const JString& value, const JString& table_out);

public:

  CalcTool();
  ~CalcTool();

  const JString name() const { return "calctool"; }

  bool run(const JParser& prm);

  void helpText(JTextStream& str) const;
};



#endif // _JUDETOOLS_CALCTOOL_H

