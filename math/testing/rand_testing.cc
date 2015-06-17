
#include "rand_testing.h"
#include "base/jlog.h"
#include "base/jstring.h"
#include "math/ranlecuyer.h"
#include "math/rand.h"
#include <list>
#include <algorithm>
#include "config.h"


using namespace jude::base;
using namespace jude::math;
using namespace std;



// [private]

template <typename RG>
bool Rand_Testing::uniformTest(const JString& name) {

  RG ran;

  int bin_count = 31;

    // if the chi_sq value for 31 bins is greater than this then only a 5% chance the generator is unbiased
    // i.e. the null hypothesis of an unbiased generator can be rejected
  double lim_95pct = 43.773;

  int repeats = 10000000;
  std::vector<int> bin_sum(bin_count, 0);

  int zero_count = 0;
  int one_or_above_count = 0;

  for (int i=0; i<repeats; i++) {

    double v = ran.generateDouble();
    
    if (v == 0) zero_count++;
    if (v >= 1.0) one_or_above_count++;
    else {
      bin_sum[(int) (v * bin_count)]++;
    }
  }

  double expected = ((double) repeats) / bin_count;
  double chi_sq = 0;

  for (int i=0; i<bin_count; i++) {
    double v = bin_sum[i] - expected;
    chi_sq += (v * v) / expected; 
  }

  jlog::ls << name.latin1() << " interval test: zc " << zero_count << " oac " << one_or_above_count << " chi_sq " << chi_sq << " (lim " << lim_95pct << ") ... ";

  if (one_or_above_count > 0 || chi_sq > lim_95pct) {
    jlog::ls << "Fail\n";
    return false;
  }

  jlog::ls << "Pass\n";
  return true;
}



// Rand_Testing::normalTest()

bool Rand_Testing::normalTest(const JString& func, double mean, double sd, double req_acc) {

  std::vector<double> data;

  int repeats = 1000000;
  //double v;
   
  if (func == "normal_dbn") for (int i=0; i<repeats; i++) data.push_back(normal_dbn(mean,sd));
  else for (int i=0; i<repeats; i++) data.push_back(normal_dbn_cstd3(mean,sd));

  sort (data.begin(), data.end());

  int lo_95pct = (int) (repeats * 0.025);
  int hi_95pct = (int) (repeats * 0.975);
 
  double lo_v_95pct = data[lo_95pct];
  double hi_v_95pct = data[hi_95pct];

  bool pass = true;
  pass = pass && (fabs(lo_v_95pct - mean + 1.96*sd) < 1.96 * sd * req_acc); 
  pass = pass && (fabs(hi_v_95pct - mean - 1.96*sd) < 1.96 * sd * req_acc); 

  jlog::ls << func.latin1() << " lo " << lo_v_95pct << " hi " << hi_v_95pct << " (acc " << req_acc << ") ... " << ((pass) ? "Pass" : "Fail") << "\n";

  return pass;
}



void Rand_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;

  jlog::ls << "Testing random number generators:\n";
 
  // test uniform distributions to expected by chi-squared


  if (!uniformTest<RanLEcuyer>("RanLEcuyer")) fc++;
  tc++;
  if (!uniformTest<Test_uniform_dbn>("uniform_dbn")) fc++;
  tc++;
  if (!uniformTest<Test_uniform_dbn>("uniform_dbn_cstd")) fc++;
  tc++;

  // normal_dbn passes if the 95% confidence intervals are within 1%

  if (!normalTest("normal_dbn", 0, 1, 0.01)) fc++;
  tc++;
  if (!normalTest("normal_dbn", 5, 10, 0.01)) fc++;
  tc++;

  // normal_dbn_cstd3 passes if the 95% confidence intervals are within 5%

  if (!normalTest("normal_dbn_cstd3", 0, 1, 0.05)) fc++;
  tc++;
  if (!normalTest("normal_dbn_cstd3", 5, 10, 0.05)) fc++;
  tc++;

  jlog::ls << "Random number generation testing passed " << tc-fc << " of " << tc << " tests.\n";
  
  total_count+= tc;
  fail_count += fc;
}

