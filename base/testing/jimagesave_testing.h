
// jimagesave_testing.h

#ifndef _JUDE_BASE_JIMAGESAVE_TESTING_H
#define _JUDE_BASE_JIMAGESAVE_TESTING_H

#include "base/jtestcase.h"
#include "base/jbuffer.h"

namespace jude {
namespace base {



class JImageSave_Testing : public JTestCase {

  void drawCross(JBuffer& buff, int x_size, int y_size, const JColour& back_col, const JColour& cross_col);

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::base

#endif // _JUDE_BASE_JIMAGESAVE_TESTING_H

