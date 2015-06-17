
// functiontab2.h

#ifndef _JUDE_MATH_FUNCTIONTAB2_H
#define _JUDE_MATH_FUNCTIONTAB2_H

#include "ublas.h"

namespace jude {
namespace math {



class FunctionTab2 {

protected:

  vector<double> x_data;
  vector<double> y_data;

public:

  FunctionTab2(const vector<double>& xd, const vector<double>& yd);

  double linearInterpolate(double x) const;

  void display(JTextStream& str);
};



} } // namespace jude::math

#endif // // _JUDE_MATH_FUNCTIONTAB2_H

