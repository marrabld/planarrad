
// functiontab.h

#ifndef _JUDE_MATH_FUNCTIONTAB_H
#define _JUDE_MATH_FUNCTIONTAB_H

#include "ipol.h"

namespace jude {
namespace math {



class FunctionTab {

protected:

  vector<double> tab_data;

  double x_min;
  double x_max;
  double x_scale;

public:

  double closestPoint(double x) const { return Ipol::closestPoint(tab_data, (x-x_min) * x_scale); }
  double linearInterpolate(double x) const { return Ipol::linearInterpolate(tab_data, (x-x_min) * x_scale); }

  void display(JTextStream& str);
};



template <typename F>
class FunctionTabImp : public FunctionTab {

  const F& func;

public:

  FunctionTabImp(const F& f, double x_min, double x_max, int x_points);
};



template <typename F>
FunctionTabImp<F>::FunctionTabImp(const F& f, double ix_min, double ix_max, int x_points) :
  func(f) {
  
  x_min = ix_min;
  x_max = ix_max;
  x_scale = (x_max == x_min) ? 0 : (x_points-1) / (x_max - x_min);
 
  tab_data.resize(x_points);

  for (int i=0; i<x_points; i++) {
    double x = x_min + i * (x_max-x_min) / (x_points - 1);
    tab_data(i) = func.eval(x);
  }
}



} } // namespace jude::math

#endif // _JUDE_MATH_FUNCTIONTAB_H

