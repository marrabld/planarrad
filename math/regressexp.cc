
// regressexp.cc

#include "math/regressexp.h"
#include "base/jlog.h"
#include <math.h>

using namespace jude::base;
using namespace jude::math;



// this was from
// http://mathworld.wolfram.com/LeastSquaresFittingExponential.html

void RegressExp::calculate(const std::vector< std::pair<double,double> >& data) {

  double sy = 0;
  double sxy = 0;
  double sx2y = 0;
  double sylny = 0;
  double sxylny = 0;
  int n = 0;

  std::vector< std::pair<double, double> >::const_iterator iter;

  for (iter = data.begin(); iter != data.end(); ++iter) {

    double xd = iter->first;
    double yd = iter->second;

    sy += yd;
    sxy += xd*yd;
    sx2y += xd*xd*yd;
    sylny += yd*log(yd);
    sxylny += xd*yd*log(yd);

    n++;
  }

  double denom = (sy * sx2y) - (sxy * sxy);

  B_est = ((sy * sxylny) - (sxy * sylny)) / denom;
  A_est = exp(((sx2y * sylny) - (sxy * sxylny)) / denom);

  //jlog::ls.printf("n: %d slope: %f y-intcp: %f\n",n,slope_est,y_intercep);
}

