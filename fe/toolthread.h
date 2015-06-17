
// toolthread.h

#ifndef _JUDE_FE_TOOLTHREAD_H
#define _JUDE_FE_TOOLTHREAD_H

#include <qthread.h>
#include "base/jparser.h"
#include "tool/tool.h"

using namespace jude::base;
using namespace jude::tool;

namespace jude {
namespace fe {



class ToolThread : public QThread {

  Q_OBJECT

  Tool& tool;
  JParser& prm;

public:

  ToolThread(Tool& tool, JParser& prm);
  ~ToolThread();

  void run();

  // in QT 4 QThread provides isFinished() previously was finished()
  // but for some reason this doesnt work in the QT3 compatibility layer 
#if QT_MAJOR < 4
  bool isFinished() const { return finished(); }
#endif

signals:
  bool processingFinished(bool ret_ok);
};



} } // namespace jude::fe

#endif // _JUDE_FE_TOOLTHREAD_H

