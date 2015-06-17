
// datautil.h

#ifndef _JUDE_MATH_DATAUTIL_H
#define _JUDE_MATH_DATAUTIL_H

#include "base/jlist.h"
#include "base/jsortable.h"

using namespace jude::base;

namespace jude {
namespace math {


class DataUtil {

public:

  static void autoRange(JList<JSortableDouble>& data_list, double data_frac, double disp_frac, double& v_min, double& v_max);

};


} } // namespace jude::math

#endif // _JUDE_MATH_DATAUTIL_H

