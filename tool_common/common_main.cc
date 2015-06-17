
#include #TOOLHEADER
#include "parser.h"
#include "log.h"


int main(int argc, char **argv) {

  log::init();

  Parser prm;

  prm.processCommandLine(argc-1, argv+1);

  TOOLOBJECT t;

  t.init(prm);
  return t.run(prm);
}

