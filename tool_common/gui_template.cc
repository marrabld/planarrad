
// main_template.cc

#include "tool_header_gui.h"
#include "tool/envvars.h"
#include "base/jparser.h"
#include "base/jlog.h"
#include "ds/spherepartvxh.h"
#include "ds/hemispherepartvxh.h"
#include "base/jgenericfactory.h"
#include "rad/raddirectable.h"
#include "rad/raddirecdirectable.h"
#include "rad/sbanddata.h"


#include <qapplication.h>
#include <QPlastiqueStyle>

using namespace jude::rad;
using namespace jude::ds;
using namespace jude::fe;



//JFont main_font("Sans",9,JFont::Normal,false);


int main(int argc, char **argv) {

    // set up text logging
  JLog::init();

    // this will be the outer_prm in the tool
  JParser prm;

    // these directory locations may or may not be required by the tool
    // we do our best to set them up but if theyr remain undefined it must be detected in the tool itself
  prm.setFromEnvVarIfDefined(ENV_TEMP_DIR, ENV_TEMP_DIR);
  prm.setFromEnvVarIfDefined(ENV_BANDSPEC_DIR, ENV_BANDSPEC_DIR);
  prm.setFromEnvVarIfDefined(ENV_DIRECSPEC_DIR, ENV_DIRECSPEC_DIR);
  prm.setFromEnvVarIfDefined(ENV_FILES_DIR, ENV_FILES_DIR);
  prm.setFromEnvVarIfDefined(ENV_LIBRADTRAN_DIR, ENV_LIBRADTRAN_DIR);

    // get parameters from files and overwite with command line a paramters - either will ovewrite environmental variables
  prm.processCommandLine(argc-1, argv+1);

    // default values mean we can at least set up demos or checks to run from a specific directory 
  if (!prm.isDefined(ENV_TEMP_DIR)) prm.setLabelAndValue(ENV_TEMP_DIR, ".");
  if (!prm.isDefined(ENV_BANDSPEC_DIR)) prm.setLabelAndValue(ENV_BANDSPEC_DIR, ".");
  if (!prm.isDefined(ENV_DIRECSPEC_DIR)) prm.setLabelAndValue(ENV_DIRECSPEC_DIR, ".");
  if (!prm.isDefined(ENV_FILES_DIR)) prm.setLabelAndValue(ENV_FILES_DIR, ".");
 
  int vb_level = (prm.isDefined("verbose")) ? prm.getInt("verbose") : 2;
  JLog::setVerboseLevel(vb_level);
  jlog::ls.setScientific(4);
  jlog::es.setScientific(4);

  if (jlog::vbLevel(5)) JGenericFactory<void>::instance().display(jlog::ls);

  /*
  if (!BandSpec::initAvailableList(prm.isDefined("RT_BANDSPEC_DIR")) {
    jlog::ls << "-------------------------------------------------------------------------\n";
    jlog::ls << "To add user-defined band specifications set the config directory\n"; 
    jlog::ls << "location by RT_BANDSPEC_DIR=<path> or the RT_BANDSPEC_DIR environmental variable\n"; 
    jlog::ls << "and add the specification files to the directory.\n";
    jlog::ls << "--------------------------------------------------------------------------\n";
  }

  if (!DirecSpec::initAvailableList(prm.isDefined("RT_DIRECSPEC_DIR")) {
    jlog::ls << "-------";
    jlog::ls << "To add user-defined directional discretizations set the config directory\n"; 
    jlog::ls << "location by RT_DIRECSPEC_DIR=<path> or the RT_DIRECSPEC_DIR environmental variable\n"; 
    jlog::ls << "and add the specification files to the directory.\n"; 
    jlog::ls << "-------";
  }
  */

  //JApplication::setFont(main_font);

  QApplication::setStyle(new QPlastiqueStyle);

    // create application
  QApplication* app = new QApplication(argc, argv);

    // construct and initalise the front end
  TOOLOBJECT_GUI* gui = new TOOLOBJECT_GUI(prm);
  gui->restoreAllSettings();
  gui->init();
  gui->clearInitialisationFlag();

#if QT_MAJOR < 4
  app->setMainWidget(gui);
#endif

    // ensure visible
  gui->show();

  int ret_val;
  try {
    ret_val = app->exec();
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    ret_val = 1;
  } 

  gui->saveAllSettings();
  //jlog::ls << "Exit.\n";

  /*! \todo Fix this with a static singleton */

  //SpherePartVxH::deleteTables();
  //HemiSpherePartVxH::deleteTables();

  //CubePartN::deleteTables();
  //HemiCubePartN::deleteTables();

  return ret_val;
}

