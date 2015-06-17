
// regresslin.cc

#include "math/regresslin.h"
#include "base/jlog.h"
#include <math.h>

using namespace jude::base;
using namespace jude::math;



void RegressLin::calculate(const std::vector< std::pair<double,double> >& data) {

  double sx = 0;
  double sy = 0;
  double sxy = 0;
  double sxx = 0;
  double syy = 0;
  int n = 0;

  std::vector< std::pair<double, double> >::const_iterator iter;

  for (iter = data.begin(); iter != data.end(); ++iter) {
    double xd = iter->first;
    double yd = iter->second;
    sx += xd;
    sy += yd;
    sxx += xd*xd;
    syy += yd*yd;
    sxy += xd*yd;
    n++;
  }

  double cxy = (n*sxy - sx*sy);
  double cxx = (n*sxx - sx*sx);
  double cyy = (n*syy - sy*sy);
  
    // m
  slope_est = cxy / cxx;
    // b
  y_intercep = (sy - slope_est*sx) / n;
      
  r_sq = (cxy * cxy) / (cxx * cyy);

  double re = 0;

  for (iter = data.begin(); iter != data.end(); ++iter) {
    double xd = iter->first;
    double yd = iter->second;
    double pred_yd = slope_est * xd + y_intercep;
    re += fabs(yd - pred_yd);
  }

  jlog::ls.printf("n: %d slope: %f y-intcp: %f r_sq %f\n",n,slope_est,y_intercep,r_sq);

    // regression standard error
  y_se = sqrt((syy-(sy*sy)/n) / (n-2));

    // standard error of the slope
  slope_se = y_se / sqrt(sxx-(sx*sx)/n); 

    // equation of lines for regression confidence intervals are
    // y0 +- TINV(alpha, df) * sqrt( y_se^2/n + slope_se^2(x0-mx)^2 )
    // where y0 = slope * x_0 + y_intercept
    // TINV is a function in Excel to give the t-value for df = n-2 degrees of freedom
    // alpha = 0.05 for 2-tailed 95% c.i.
    // for large n TINV(0.05) = 1.96

  jlog::ls.printf("slope_se %f y_se %f df %d mx %f\n",slope_se,y_se,n-2,sx/n);
  jlog::ls.printf("mean residual y error %f\n",re/n);
}



