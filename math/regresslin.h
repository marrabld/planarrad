
// regresslin.h

#ifndef _JUDE_MATH_REGRESSLIN_H
#define _JUDE_MATH_REGRESSLIN_H

#include <vector>

namespace jude {
namespace math {



class RegressLin {

  double slope_est;
  double y_intercep;      
  double r_sq;
  double y_se;
  double slope_se; 

public:

  void calculate(const std::vector< std::pair<double,double> >& data);

  double slope() const { return slope_est; }
};



} } // namespave jude::math

#endif // _JUDE_MATH_REGRESSLIN_H


