
// rand.h

#ifndef _JUDE_MATH_RAND_H
#define _JUDE_MATH_RAND_H

#include <math.h>
#include "ranlecuyer.h"

namespace jude {
namespace math {


RanLEcuyer& standardRanGen();

void random_seed(long i);

bool rand_bool_cstd();

int rand_int(int max_plus_one);
inline int rand_int(int min_inc, int max_exc) { return rand_int(max_exc-min_inc) + min_inc; }

double uniform_dbn(double min, double exc_max);
double uniform_dbn_cstd(double min, double exc_max);

double normal_dbn(double mean, double sd);

double normal_dbn_m48(double mean, double sd);
double normal_dbn_cstd3(double mean, double sd);

inline double standard_normal_value(double x) { return (1/sqrt(2*M_PI)) * exp(-(x*x) / 2); }
double normal_value(double mean, double sd, double x);



template <typename RG>
double uniform_dbn(double min, double exc_max, RG& rg) {
  return min + rg.generateDouble() * (exc_max-min);
}



template <typename RG>
double normal_dbn(double mean, double sd, RG& rg) {
 
  double v = 0;
 
  for (int i=0;i<12;i++) v += uniform_dbn(0.0, 1.0, rg);
  v = v - 6;

   // centre on mean and change standard deviation
  v = mean + (v*sd);
 
  return v;
}



/*!
\fn standard_normal_value()
\brief Returns the y-value for the x-value \a x for the standard normal probability distribution function, i.e. mean = 0 and S.D. = 1.

\todo Build test cases to check this.
*/



} } // namespace jude::math

#endif // _JUDE_MATH_RAND_H

