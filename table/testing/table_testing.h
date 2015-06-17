
// table_testing.h

#ifndef _JUDE_TABLE_TABLE_TESTING_H
#define _JUDE_TABLE_TABLE_TESTING_H

#include "base/jtestcase.h"
#include "base/jstring.h"


using namespace jude::base;

namespace jude {
namespace table {



class Table_Testing : public JTestCase {

public:

  void runTests(int& total_count, int& fail_count);
};



} } // namespace jude::table

#endif // _JUDE_TABLE_TABLE_TESTING_H

