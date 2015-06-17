
// toolthread.cc

#include "fe/toolthread.h"

using namespace jude::fe;



/* \brief Constructs a thread for running the specified Tool instance \a tool from an assocted JParser \a prm */

ToolThread::ToolThread(Tool& tool, JParser& prm) :
  tool(tool),
  prm(prm) { }



/* \brief Destructor. */

ToolThread::~ToolThread() { 
  // nothing to do
}



/* \brief This is called when the thread is started. */

void ToolThread::run() {
  bool ret_ok = tool.run(prm);
  emit processingFinished(ret_ok);
}

