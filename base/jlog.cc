
#include "jlog.h"
#include "jconsole.h"

using namespace jude::base;



// global log streams for info and errors respectively

JLog jlog::ls;
JLog jlog::es;

JLog jlog::con;
JLog jlog::f;


// static member

int JLog::verbose_level;


// JLog::init() [static]
// by default set up info and errors to the console

void JLog::init() {

  jlog::ls.addDevice(&JConsole::standard);
  jlog::ls.setScientific(4);

  jlog::es.addDevice(&JConsole::standard);
  jlog::es.setScientific(4);

  jlog::con.addDevice(&JConsole::standard);
  jlog::con.setScientific(4);

  jlog::f.setScientific(4);

  JLog::setVerboseLevel(2);
}



void JLog::setLogFile(JFile* log_file) {

  jlog::ls.addDevice(log_file);
  jlog::es.addDevice(log_file);
  jlog::f.addDevice(log_file);
}

