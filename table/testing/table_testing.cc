
#include "table_testing.h"
#include "base/jlog.h"
#include "base/jstring.h"
#include "ds/spherepartvxh.h"
#include "ds/hemispherepartvxh.h"
#include "table/directable.h"
#include <vector>
#include <string.h>

using namespace jude::base;
using namespace jude::table;
using namespace std;



void Table_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;
  
  double tp_half_data[11] = { 0, 5, 15, 25, 35, 45, 55, 65, 75, 85, 90 };
  std::vector<double> tp_half(11);
  memcpy(&tp_half[0], tp_half_data, tp_half.size() * sizeof(double));
 
  HemiSpherePartVxH hds(9, 24, tp_half);

  DirecTable<double, HemiSpherePartVxH> dt(hds);

  total_count += tc;
  fail_count += fc;
}

