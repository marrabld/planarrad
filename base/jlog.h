
// jlog.h

#ifndef _JUDE_BASE_LOG_H
#define _JUDE_BASE_LOG_H

#include "base/jtextstream.h"
#include "base/jmultipleoutdevice.h"
#include "base/jfile.h"

namespace jude {
namespace base {


/*! \brief Used for general output of text information to the console or a log file.

This subclass of JTextStream provides for multiple bound output devices so writing to a JLog can simultaneously write to the console and a log file. Two singly instantiated JLog objects are provided which can be accessed through the namespace \c jlog. They are \c jlog::ls for general of info logging and \c jlog::es for errors. These are to be prefered instead of the standard library printf(). Since JLog provides JTextStream::printf() replacement of the use of printf() in code is trivial.

Note that all programs must call the static member JLog::init() prior to use. This will normally be the first operation in main().
*/


class JLog : public JTextStream {

    // the group of out devices treated as a single device 
  JMultipleOutDevice out_devs;

  // general rules for verbose level 

  // 0
  // no console output at all

  // 1
  // display main final output to console or summary info on it
  // display a message about any file that is saved
  // display warnings about input parameters starting "WARNING:"

  // 2 (default)
  // display useful intermediate steps and input parameter checks
  // display progress counters 

  // 3
  // display a message about any file that is read
  // display more lengthy stuff that would be too much for normal use
  // display progress statements about major processing steps eg. "Calculating matrices... " etc.

  // 4 - 5
  // display more lengthy info and minor progress statements but still things that could concievably be of use to user
  // eg. contents of loaded files

  // 6 - 9 
  // debug or temporary stuff that would mean nothing to a user
  // FactoryIDs only appear at level 6 or higher

  static int verbose_level;

public:

  static void init();

  static void setLogFile(JFile* log_file);

  JLog() { 
    setDevice(&out_devs); 
    setScientific(JUDE_DEFAULT_OUTPUT_PREC);
  }

  void addDevice(JIODevice* dev) { out_devs.addDevice(dev); }

  static int verboseLevel() { return verbose_level; }
  static void setVerboseLevel(int v) { verbose_level = v; }

    // depreceated
  static bool vbLevel(int v) { return verbose_level >= v; }
    // use this
  static bool vb(int v) { return verbose_level >= v; }
};



// use a namespace here so in code ls and es can be used directly

namespace jlog {

  extern JLog ls;
  extern JLog es;

  extern JLog con;
  extern JLog f;

  inline int verboseLevel() { return JLog::verboseLevel(); }
  inline bool vb(int v) { return JLog::vb(v); }
  inline bool vbLevel(int v) { return JLog::vbLevel(v); }
}



} } // namespace jude::base

#endif // _JUDE_BASE_LOG_H

