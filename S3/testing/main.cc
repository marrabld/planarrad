
#include "base/jlog.h"


namespace jude { namespace S3 { } }

using namespace jude::base;
using namespace jude::S3;



int main() {

  JLog::init();

  int total_count = 0;
  int fail_count = 0;


  return (fail_count == 0) ? 0 : 1;
}

