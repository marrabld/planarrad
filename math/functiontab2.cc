
#include "math/functiontab2.h"
#include "math/ipol.h"
#include "base/jtexttable.h"


using namespace jude::math;


FunctionTab2::FunctionTab2(const vector<double>& xd, const vector<double>& yd) :
  x_data(xd),
  y_data(yd) { }



double FunctionTab2::linearInterpolate(double x) const {
  return Ipol::linearInterpolate(x, y_data, x_data); 
}



void FunctionTab2::display(JTextStream& str) {
  jlog::ls << "HERE " << (int)x_data.size() << "\n";
  for (unsigned int i=0; i<x_data.size(); i++) {
    str << x_data(i) << " : " << y_data(i) << "\n";
  }
}

