
// regressexp.h

#ifndef _JUDE_MATH_REGRESSEXP_H
#define _JUDE_MATH_REGRESSEXP_H

#include <vector>

namespace jude {
namespace math {



/*! Fits supplied data to an exponential function y = A x exp(Bx). */ 

class RegressExp {

  double A_est;
  double B_est;

public:

  void calculate(const std::vector< std::pair<double,double> >& data);

  double A() const { return A_est; }
  double B() const { return B_est; }
};



} } // namespave jude::math

#endif // _JUDE_MATH_REGRESSEXP_H


