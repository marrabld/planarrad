
#include "datautil.h"

using namespace jude::math;



 // fix the middle data_frac % to the middle disp_frac % of the range

void DataUtil::autoRange(JList<JSortableDouble>& data_list, double data_frac, double disp_frac, double& v_min, double& v_max) {

    // sort the list
  data_list.sort();
    // the lower 10 per cent plus one to ensure is at least one entry
  int lower_num = (int) (data_list.count() * ((1 - data_frac) / 2));
  int upper_num = (int) (data_list.count() - ((lower_num) ? lower_num : 1));

    // values at the upper and lower bounds
  v_min = data_list.at(lower_num)->value();
  v_max = data_list.at(upper_num)->value();
 
  double ex = ((v_max - v_min) * ((1 - disp_frac) / disp_frac)) / 2;
  v_min -= ex;
  v_max += ex;
}

