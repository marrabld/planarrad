
#include "math/functiontab.h"

using namespace jude::math;



void FunctionTab::display(JTextStream& str) {

  for (unsigned int i=0; i<tab_data.size(); i++) {
    double x =  x_min + i * (x_max-x_min) / (tab_data.size() - 1);
    str << i << ": " << x << "  " << tab_data(i) << "\n";
  }
}

