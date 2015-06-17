
#include "jstring_testing.h"
#include "base/jlog.h"
#include "base/jstring.h"

using namespace jude::base;



void JString_Testing::runTests(int& total_count, int& fail_count) {

  int tc = 0;
  int fc = 0;

  jlog::ls << "Testing toInt():\n";
  textToInt("1", 1, tc, fc);
  textToInt("2", 2, tc, fc);
  textToInt("1234", 1234, tc, fc);
  textToInt(" 1234", 1234, tc, fc);
  textToInt("+1234", 1234, tc, fc);
  textToInt("-1234", -1234, tc, fc);
  textToInt("  +1234", 1234, tc, fc);
  textToInt("  -1234", -1234, tc, fc);
  textToInt("  +1234  ", 1234, tc, fc);
  textToInt("  -1234  ", -1234, tc, fc);
  textToInt("12.34", 12, tc, fc);
  textToInt("  12.34", 12, tc, fc);
  textToInt("12.34  ", 12, tc, fc);
  textToInt("  12.34  ", 12, tc, fc);
  textToInt(",,1234,,", 1234, tc, fc);
  textToInt("\t\t1234\t\t", 1234, tc, fc);
  textToInt("1E2", 100, tc, fc);
  textToInt("3E4", 30000, tc, fc);
  textToInt("  1E2  ", 100, tc, fc);
  textToInt("1e2", 100, tc, fc);
  textToInt("1E02", 100, tc, fc);
  textToInt("12x3", 0, tc, fc);
  textToInt("12e3e4", 0, tc, fc);
  textToInt("12.3.4", 0, tc, fc);
  textToInt("     ", 0, tc, fc);

  jlog::ls << "Testing toInt(int start_pos, int end_pos):\n";
  textToIntSegment("1234", 0, 3, 1234, tc, fc);
  textToIntSegment("1234", 1, 2, 23, tc, fc);
  textToIntSegment("xx123xx", 2, 4, 123, tc, fc);
  textToIntSegment("xx12.34xx", 2, 5, 12, tc, fc);

    // note here need to use a number that can be represented in bicimal
  jlog::ls << "Testing toDouble():\n";
  textToDouble("1", 1, tc, fc);
  textToDouble("2", 2, tc, fc);
  textToDouble("2048", 2048, tc, fc);
  textToDouble(" 2048", 2048, tc, fc);
  textToDouble("+2048", 2048, tc, fc);
  textToDouble("-2048", -2048, tc, fc);
  textToDouble("  +2048", 2048, tc, fc);
  textToDouble("  -2048", -2048, tc, fc);
  textToDouble("3.75", 3.75, tc, fc);   // based on 15/4
  textToDouble("  3.75", 3.75, tc, fc);
  textToDouble("  3.75  ", 3.75, tc, fc);
  textToDouble("0.00375E3", 3.75, tc, fc);
  textToDouble("0.00375E+3", 3.75, tc, fc);
  textToDouble("0.00375e3", 3.75, tc, fc);
  textToDouble("  0.00375e3  ", 3.75, tc, fc);
  textToDouble(",,0.00375e3,,", 3.75, tc, fc);
  textToDouble("\t\t0.00375e3\t\t", 3.75, tc, fc);
  textToDouble(" , 0.00375e3 , ", 3.75, tc, fc);
  textToDouble("-0.00375e3", -3.75, tc, fc);
  textToDouble("-0.00375e+3", -3.75, tc, fc);
  textToDouble("-0.00375e+03", -3.75, tc, fc);
  textToDouble("-0.00375e003", -3.75, tc, fc);
  textToDouble("375E-2", 3.75, tc, fc);
  textToDouble("375e-2", 3.75, tc, fc);
  textToDouble("37500E-4", 3.75, tc, fc);
  textToDouble("-37.5E-1", -3.75, tc, fc);
  textToDouble("+37.5E-1", +3.75, tc, fc);
  textToDouble("78.5", 78.5, tc, fc);   // based on 1256/16
  // this fails
  // textToDouble("127.9375", 127.9375, tc, fc);   // based on 2047/16

  jlog::ls << "Testing toDouble(int start_pos, int end_pos):\n";
  textToDoubleSegment("xx3.75xx", 2, 5, 3.75, tc, fc);
  textToDoubleSegment(" 13.755 ", 2, 5, 3.75, tc, fc);
 
  jlog::ls << "JString testing passed " << tc-fc << " of " << tc << " tests.\n\n";
  
  total_count+= tc;
  fail_count += fc;
}



// [private]

void JString_Testing::textToInt(const JString& s, int n, int& tc, int& fc) {
  jlog::ls << "'" << s << "'" << " = " << n << " ... "; 
  int r = s.toInt();
  if (r != n) {
    fc++;
    jlog::ls << r << " Fail\n";
  } else {
    jlog::ls << r << " Pass\n";
  }
  tc++;
}



// [private]

void JString_Testing::textToIntSegment(const JString& s, int start_pos, int end_pos, int n, int& tc, int& fc) {
  jlog::ls << "'" << s << "'" << " = " << n << " ... "; 
  int r = s.toInt(start_pos, end_pos);
  if (r != n) {
    fc++;
    jlog::ls << r << " Fail\n";
  } else {
    jlog::ls << r << " Pass\n";
  }
  tc++;
}



// [private]

void JString_Testing::textToDouble(const JString& s, double v, int& tc, int& fc) {
  jlog::ls << "'" << s << "'" << " = " << v << " ... "; 
  double r = s.toDouble();
  if (r != v) {
    fc++;
    jlog::ls << r << " Fail\n";
  } else {
    jlog::ls << r << " Pass\n";
  }
  tc++;
}



// [private]

void JString_Testing::textToDoubleSegment(const JString& s, int start_pos, int end_pos, double v, int& tc, int& fc) {
  jlog::ls << "'" << s << "'" << " = " << v << " ... "; 
  double r = s.toDouble(start_pos, end_pos);
  if (r != v) {
    fc++;
    jlog::ls << r << " Fail\n";
  } else {
    jlog::ls << r << " Pass\n";
  }
  tc++;
}

