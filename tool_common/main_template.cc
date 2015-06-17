
// main_template.cc

#include "tool_header.h"
#include "base/jparser.h"
#include "tool/envvars.h"
#include "base/jlog.h"
//#include "ds/spherepartvxh.h"
//#include "ds/hemispherepartvxh.h"
//#include "base/jgenericfactory.h"
//#include "rad/raddirectable.h"
//#include "rad/raddirecdirectable.h"
//#include "rad/sbanddata.h"
#include "config.h"


//using namespace jude::rad;
//using namespace jude::ds;



int main(int argc, char **argv) {

    // register types
    // somewhere in all programs there must occur a list of all the static polymorphic types that can occur
    // and which need to be saved and loaded from file when only base type is known
    // a consequence of this inclusing is all tools must ling to libjudeds
    // maybe they can be included somewhere else
    /*! \todo Move factory initialisations to functions within appropriate libraries. */
  //  JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  // JFactory< RadDirecImp< SBandData>, RadDirecTable< SBandData, SpherePartVxH > >::instance().reg();
  // JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, HemiSpherePartVxH > >::instance().reg();
  // JFactory< RadDirecDirecImp< SBandData>, RadDirecDirecTable< SBandData, SpherePartVxH > >::instance().reg();


  JLog::init();

  JParser prm;

    // load these from environmental variables but they may be owverwitten from the run parameters
  prm.setFromEnvVarIfDefined(ENV_TEMP_DIR, ENV_TEMP_DIR);
  prm.setFromEnvVarIfDefined(ENV_BANDSPEC_DIR, ENV_BANDSPEC_DIR);
  prm.setFromEnvVarIfDefined(ENV_DIRECSPEC_DIR, ENV_DIRECSPEC_DIR);
  prm.setFromEnvVarIfDefined(ENV_FILES_DIR, ENV_FILES_DIR);
  prm.setFromEnvVarIfDefined(ENV_LIBRADTRAN_DIR, ENV_LIBRADTRAN_DIR);

  prm.processCommandLine(argc-1, argv+1);

    // set defaults to current directory
  if (!prm.isDefined(ENV_TEMP_DIR)) prm.setLabelAndValue(ENV_TEMP_DIR,".");
  if (!prm.isDefined(ENV_BANDSPEC_DIR)) prm.setLabelAndValue(ENV_BANDSPEC_DIR,".");
  if (!prm.isDefined(ENV_DIRECSPEC_DIR)) prm.setLabelAndValue(ENV_DIRECSPEC_DIR,".");
  if (!prm.isDefined(ENV_FILES_DIR)) prm.setLabelAndValue(ENV_FILES_DIR,".");

    // default verbosity is 2
  int vb_level = (prm.isDefinedAndNotEmpty("verbose")) ? prm.getInt("verbose") : 2;
  JLog::setVerboseLevel(vb_level);
    // default precision is 4
  int prec = (prm.isDefinedAndNotEmpty("log_output_precision")) ? prm.getInt("log_output_precision") : 4;
  jlog::ls.setScientific(prec);
  jlog::es.setScientific(prec);

  if (jlog::vbLevel(5)) JGenericFactory<void>::instance().display(jlog::ls);

  TOOLOBJECT t;

  //t.init(prm);
  //t.run(prm);
  int ret_val = t.run(prm) ? 0 : 1;

  return ret_val;
}

