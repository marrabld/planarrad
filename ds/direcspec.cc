
// direcspec.cc

#include "ds/direcspec.h"
#include "base/jparser.h"
#include "base/jdir.h"
#include "base/jlog.h"

using namespace jude::base;
using namespace jude::ds;



  // static list of pre-loaded DirecSpec's used for GUI applications
JList<DirecSpec> DirecSpec::available_list;
  // the default in the list
DirecSpec* DirecSpec::default_ds_spec = 0;



// DirecSpec::initAvailableList()

void DirecSpec::initAvailableList() {
    // will create DirecSpec's here using new so will need to be deleted when app quits
  available_list.setAutoDelete(true);
    // no default 
  default_ds_spec = 0;
    // clear list
  available_list.clear();
}



// DirecSpec::addToAvailableList()

DirecSpec* DirecSpec::addToAvailableList(const JParser& prm, const JString& prefix, bool make_default) {

  bool preferred_default;

    // first try to see if there is one directly specified in the parser or via a file
  DirecSpec* ds_spec = create(prm, prefix, preferred_default);
    // quit if none found
  if (!ds_spec) return 0;
    // log
  if (jlog::vbLevel(5)) jlog::ls << "Adding directional specification '" << ds_spec->name() << "' to list\n";
    // add to list
  available_list.append(ds_spec);
    // and make default if requested from specification and no previous default
    // or always if demanded from caller
  if ((preferred_default && !default_ds_spec) || make_default) default_ds_spec = ds_spec;
    // return pointer
  return ds_spec;
}



// DirecSpec::initAvailableList() [static]
// returns false if the directory does not exist

bool DirecSpec::addToAvailableList(const JString& dir_path) {

  if (jlog::vbLevel(5)) jlog::ls << "Looking for directional structure specification \".dsf\" files in '" << dir_path << "'.\n";

    // get list of files in the directory
  JDir dir(dir_path);

  if (!dir.exists()) return false;

  JList<JString> entry_list;
  entry_list.setAutoDelete(true);
  dir.getEntryList(entry_list);

  int found_count = 0;

    // go through each file
  JString* fp = entry_list.first();
  while (fp) {

      // if ends in ".txt" then try to load it as a direc spec
    if (fp->right(4) == ".dsf") {
      bool preferred_default;
        // try to create from file
      DirecSpec* ds_spec = createFromFile(dir_path + "/" + *fp, preferred_default);
        // if went ok then add to available list
      if (ds_spec) { 
  	  // add to list
	available_list.append(ds_spec); 
	  // and make default if requested
	if (preferred_default && !default_ds_spec) { default_ds_spec = ds_spec; }
	found_count++; 
      }
    }
      // next file in directory
    fp = entry_list.next();
  }

    // info to log
  if (jlog::vbLevel(5)) {
    if (found_count==0) jlog::ls << "None found!\n";
    else jlog::ls << "Found " << found_count << ".\n";
  }

  return true;
}



// DirecSpec::addDefaultToAvailableList() [static]

bool DirecSpec::addDefaultToAvailableList() {

    // if got some in the list then don't need to create a default
  if (available_list.count() > 0) return false;

  if (jlog::vbLevel(5)) jlog::ls << "Creating default HL Standard.\n";

  DirecSpec* ds_spec = new DirecSpec("");
  ds_spec->setName("HL Standard");
  ds_spec->setCode("HL18x24");
  ds_spec->prm.setLabelAndValue("ds_name","HL Standard");
  ds_spec->prm.setLabelAndValue("ds_code","HL18x24");
  ds_spec->prm.setLabelAndValue("partition","sphere");
  ds_spec->prm.setLabelAndValue("vn","18");
  ds_spec->prm.setLabelAndValue("hn","24");
  ds_spec->prm.setLabelAndValue("theta_points","0,5,15,25,35,45,55,65,75,85,90,95,105,115,125,135,145,155,165,175,180");

    // add to list
  available_list.append(ds_spec);
    // make default
  default_ds_spec = ds_spec;

  return true;
}



// DirecSpec::create() [static]
 
DirecSpec* DirecSpec::create(const JParser& prm, const JString& prefix) {
  bool dummy;
  return create(prm, prefix, dummy);
}



// DirecSpec::create() [static]

DirecSpec* DirecSpec::create(const JParser& prm, const JString& prefix, bool& preferred_default) {

    // if 'partition' is defined in prm then the specification is in prm
  if (prm.isDefined(prefix+"partition")) {

      // create a band spec with no file path
    DirecSpec* ds_spec = new DirecSpec("");
      // read it directly from the parser - return if went ok
    if (ds_spec->readFromParser(prm, prefix, preferred_default)) return ds_spec;
      // otherwise delete the band spec
    delete ds_spec;
      // quit with failure
    return 0;

    // if 'direc_spec_fp' is defined then the supplied file is the parser specification
  } else if (prm.isDefined(prefix+"direc_spec_fp")) {

      // make a file descriptor for the file to read
    return createFromFile(prm.getString(prefix+"direc_spec_fp"), preferred_default);
  }

    // neither 'partition' nor 'direc_spec_fp' defined so quit with failure
  return 0;
}
 


// DirecSpec::createFromFile() [static]

DirecSpec* DirecSpec::createFromFile(const JString& file_path, bool& preferred_default) {
 
  if (jlog::vbLevel(5)) jlog::ls.printf("Loading directional structure file: '%s'.\n", file_path.latin1());
 
    // make a parser from the file
  JParser ds_spec_prm;

  if (ds_spec_prm.readFile(file_path, 5)) {
      // create a band spec with file path
    DirecSpec* ds_spec = new DirecSpec(file_path);
      // read it from the new parser with no prefix - return if went ok
    if (ds_spec->readFromParser(ds_spec_prm, "", preferred_default)) return ds_spec;
      // otherwise delete the band spec
    delete ds_spec;
  }

    // couldn't load
  if (jlog::vbLevel(5)) jlog::ls.printf("File '%s' ignored.\n", file_path.latin1());

  return 0;
}



// DirecSpec::(constructor)
// don't read in data in constructor because we need to know if went wrong

DirecSpec::DirecSpec(const JString& fp) :
  filepath_str(fp),
  name_str(""),
  code_str("") { }



// DirecSpec::build()
// returns false if something went wrong
// if unsupported = true then directional discretization is not supported in this version

bool DirecSpec::readFromParser(const JParser& init_prm, const JString& prefix, bool& preferred_default) {

    // try to get a base directory from the parser, i.e. the directory the parser file was in
  JString base_dir = prm.baseDir() + "/";

  if (jlog::vbLevel(5)) init_prm.displayList(jlog::ls);

  if (jlog::vbLevel(5)) jlog::ls.printf("Directional specification base directory '%s'.\n",base_dir.latin1());

  if (!init_prm.isDefined(prefix+"partition")) { 
    if (jlog::vbLevel(5)) jlog::ls << "Parameter '" << prefix << "partition' not defined.\n"; 
    return false; 
  }

  if (init_prm.isDefined(prefix+"ds_name")) name_str = init_prm.getString(prefix+"ds_name");
  if (init_prm.isDefined(prefix+"ds_code")) code_str = init_prm.getString(prefix+"ds_code");

  prm.setLabelAndValue("partition", init_prm.getString(prefix+"partition"));
  if (init_prm.isDefined(prefix+"storage")) prm.setLabelAndValue("storage", init_prm.getString(prefix+"storage"));
  if (init_prm.isDefined(prefix+"vn")) prm.setLabelAndValue("vn", init_prm.getString(prefix+"vn"));
  if (init_prm.isDefined(prefix+"hn")) prm.setLabelAndValue("hn", init_prm.getString(prefix+"hn"));
  if (init_prm.isDefined(prefix+"theta_points")) prm.setLabelAndValue("theta_points", init_prm.getString(prefix+"theta_points"));
  if (init_prm.isDefined(prefix+"theta_points_fp")) {
    JString theta_points_fp = init_prm.getString(prefix+"theta_points_fp");
    if (theta_points_fp.left(1) != "/") theta_points_fp.prepend(base_dir);
    prm.setLabelAndValue("theta_points_fp", theta_points_fp);
  }
    // the specification can specify if it is the preferred default in a gui context
    // this may be ignored if another band spec or the tool sets a different default
  preferred_default = (init_prm.isDefinedAndTrueBool(prefix+"preferred_default"));

  return true;
}



// DirecSpec::addToParser()

void DirecSpec::addToParser(JParser& targ_prm, const JString& prefix) const {

    // if was created from a file then just pass the file
    // since this is used when a gui front-end invokes a tool it will always be able to find the same file
  if (!filepath_str.isEmpty()) { targ_prm.setLabelAndValue(prefix+"direc_spec_fp", filepath_str); return; }

  targ_prm.appendOrOverwriteEntries(prm, prefix);
}



// DirecSpec::helpText() [static]

void DirecSpec::helpText(JTextStream& str) {

  str << "<prefix>ds_name      = <string>  (name of the directional structure scheme)\n";
  str << "<prefix>ds_code      = <string>  (short code to use to id full spherical versions directional structure scheme)\n";
  str << "<prefix>partition    = sphere  (currently only spherical partitions are supported)\n";
  str << "<prefix>storage      = table   (currently only table storage is supported, optional)\n";
  str << "<prefix>vn           = <integer>  (number of vertical quads in a sphere excluding endcaps)\n";
  str << "<prefix>hn           = <integer>  (number of horizontal quads)\n";
  str << "<prefix>theta_points         = <r0,r1,r2, ... rn>  (theta values of vertical quad edges over full sphere, 0 to 180, optional)\n";
  str << "<prefix>theta_points_fp      = <filename>  (file of theta values of vertical quad edges over full sphere, 0 to 180, optional)\n";
  str << "<prefix>preferred_default    = yes | no  (request this be the default directional specification in a gui context)\n\n";
  str << "Note if theta points are not specified a 'regular' directional structure is constructed. In this case all quads have the same\n";
  str << "zenith angular height and the total end cap angle is also this angular size, equal to pi / (vn+1) radians.\n\n";
}


//  str << "<prefix>ds_hemi_code = <string>  (short code to use to id hemisphere versions of the directional structure scheme)\n";
//  str << "<prefix>hvn          = <integer> (number of vertical quads in a hemisphere excluding endcap)\n";
//  str << "<prefix>hemi_theta_points    = <r0,r1,r2, ... rn>  (theta values of vertical quad edges for hemisphere, 0 to 90, optional)\n";
//  str << "<prefix>hemi_theta_points_fp = <filename>  (file of theta values of vertical quad edges for hemisphere, 0 to 90, optional)\n";


// DirecSpec::display()

void DirecSpec::display(JTextStream& str) const {
  // to do
}

