
// wltool.cc

#include "wltool.h"
#include "config.h"
#include "wl/archivefile.h"

using namespace jude::base;
using namespace jude::wl;



// WLTool::(constructor)

WLTool::WLTool() : 
  Tool(), 
  config(new SensorConfig),
  af(0),
  sensor_setup_ok(false),
  records_extracted_ok(false),
  loading_processed_channel(-1) { init(); }



// WLTool::(destructor)

WLTool::~WLTool() {
  delete config;
  delete af;
}



// WLTool::init()

void WLTool::init() {

  static bool done_init = false;
  if (done_init) return;

  done_init = true;
}



// WLTool::run()

bool WLTool::run(const JParser& prm) {

    // basic initialisation common to all tools
  if (Tool::init(prm)) return true;
  Tool::initRandom(prm);

    // initialise completion count
  Tool::pct_complete = 0;
  
  loading_processed_channel = -1;

    // get the operation
    // preview - does not build any internal data strictires, just counts packets
    // extract - builds both packet and record lists, type of record list depends on sensor setup
    // extract_and_merge - builds both packet and record lists and merged list
  JString op = prm.getStringOrEmpty("operation");
  if (op.isEmpty()) { op = "preview"; }

    // this operation is used by the gui to load in processed HOCR to an already present archive file
  if (op=="load_processed" && af) {
    loading_processed_channel = prm.getInt("channel_index");
    return af->loadProcessed(loading_processed_channel, prm);
  }

  if (op=="merge" && af) {
    return doMerge(prm);
  }

  if (op != "preview" && op != "extract" && op != "extract_and_merge") { 
    jlog::es << "Operation '" << op << "' not supported.\n";
    return false;
  }

  bool preview_only = (op == "preview");
  bool do_merge = (op == "extract_and_merge");

  if (!prm.isDefinedAndTrueBool("preloaded_sensor_setup")) { if (!buildSensorConfig(prm)) return false; }    
  else config->clearAllProcessed();

    // archive filepath must always be supplied
  JString archive_fp = prm.getStringOrEmpty("archive_fp");
    // should never get this in GUI
  if (archive_fp.isEmpty()) { jlog::es << "Path to archive file must be specified with 'archive_fp'.\n"; return false; }
    // log whatever is being done
  if (jlog::vbLevel(2)) jlog::ls << ((preview_only) ? "Preview read of" : "Extracting") << " '" << archive_fp << "'\n";

  if (!loadArchiveFile(archive_fp, preview_only, prm)) return false;
 
    // do merging
  if (do_merge) {

    if (!doMerge(prm)) return false;

    JString fp(prm.getStringOrEmpty("merge_file_save_fp"));
    if (!fp.isEmpty()) if (!saveMergedFile(fp)) return false;  
  }

    // finalise completion count
  Tool::pct_complete = 100;

  return true;
}



double WLTool::pctComplete() const {

  if (!af) return 0;

  if (loading_processed_channel < 0) {
    
    double pct = af->pctReadCompleted();
    if (pct < 100) return pct/2;
  
    double pct_sum = 0;
    int pct_count = 0;
    
    for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
      if (af->channel(i).recordCount() > 0) { pct_count++; pct_sum += af->channel(i).pctProcessedDone(); }
    }
    
    return 50 + pct_sum / (2*pct_count);
 
  } else {
    return config->sensor(loading_processed_channel).pctRecordsProcessed();
  }
}



void WLTool::stopProcessing() { 
  if (!af) return;
  af->stopProcessing();
  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {
    af->channel(i).stopProcessing();
    config->sensor(i).stopProcessing();
  }
}



void WLTool::deleteArchiveFile() { 
  delete af; 
  af = 0; 
  records_extracted_ok = false;
  config->clearAllProcessed();
}



bool WLTool::buildSensorConfig(const JParser& prm) {

  if (jlog::vb(5)) { jlog::ls << "WLTool::buildSensorConfig()\n"; prm.displayList(jlog::ls); }

  SensorConfig* new_config = new SensorConfig(prm);

  if (!new_config->statusOk()) { delete new_config; return false; }

    // swap in the new config
  deleteArchiveFile();
  delete config;
  config = new_config;

  sensor_setup_ok = true;

  return true;
}



bool WLTool::loadArchiveFile(const JString& archive_fp, bool preview_only, const JParser& prm) {

  if (jlog::vb(5)) { jlog::ls << "WLTool::loadArchiveFile()\n"; prm.displayList(jlog::ls); }

  records_extracted_ok = false;

    // create archive file
  delete af;
  af = new ArchiveFile(*config);

    // read the packets and either just count or build packet and record lists
  if (!af->readPackets(archive_fp, preview_only)) return false;

  if (jlog::vbLevel(2)) af->display(jlog::ls);

    // if just got records
  if (!preview_only) {
      // process the records to final level (ACS spectra etc) or possibly load in synced processed data (HOCR)
    if (!af->processOrLoadProcessed(prm)) return false;
      // write out the channel files (record lists) if requested
    JString base_fp(prm.getStringOrEmpty("channel_files_save_fp"));
    if (!base_fp.isEmpty()) saveChannelFiles(base_fp);
  }

  records_extracted_ok = true;

  return true;
}



bool WLTool::doMerge(const JParser& prm) {

  int merge_chan = 0;
  if (prm.isDefined("merge_reference_channel")) merge_chan = prm.getInt("merge_reference_channel")-1;
  if (merge_chan < 0 || merge_chan>=HARDWARE_CHANNEL_COUNT) {
    jlog::es << "Merge reference channel " << merge_chan+1 << " is invalid, should be 1 to " << HARDWARE_CHANNEL_COUNT << ".\n";
    return false;
  }
  bool inc_flags[HARDWARE_CHANNEL_COUNT];
  
  if (prm.isDefined("merge_file_channels")) {
    for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) inc_flags[i] = false;
    std::vector<int> v = prm.getVectorOfInts("merge_file_channels");
    for (unsigned int i=0; i<v.size(); i++) if (v[i] > 0 && v[i] <= HARDWARE_CHANNEL_COUNT) inc_flags[v[i]-1] = true;
  } else {
    for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) inc_flags[i] = true;
  }
    
  //inc_flags[0] = false;
  
  af->doMerge(prm, merge_chan, inc_flags);

  return true;
}



bool WLTool::saveMergedFile(const JString& fp) {

  if (!af) return true;

  JFile f(fp);

  if (f.open(JIO_WriteOnly)) {
    JTextStream str(&f);
    str.setScientific(5);
    af->channelMerge().writeProcessed(str);
  } else {
    jlog::es << "Couldn't open file '" << fp << "' to write.";
    return false;
  }

  f.close();

  return true;
}



// WLTool::channelFilePath()

const JString WLTool::channelFilepath(const JString& base_fp, int i) {
  return base_fp + "_chan" + JString::number(i+1) + "_" + config->sensor(i).typeText() + config->sensor(i).fileExtension();
}



// WLTool::saveChannelFiles()

bool WLTool::saveChannelFiles(const JString& base_fp) {

  for (int i=0; i<HARDWARE_CHANNEL_COUNT; i++) {

    if (af->channel(i).byteCount() == 0) continue;

    JString fp(channelFilepath(base_fp, i));

    if (jlog::vbLevel(1)) jlog::ls << "Writing channel " << (i+1) << " file '" << fp << "'\n";

    if (!saveChannelFile(fp,i)) return false;
  }

  return true;
}



bool WLTool::saveChannelFile(const JString& fp, int i) {

  JFile f(fp);
  if (!f.open(JIO_WriteOnly)) return false;
  
  JDataStream str(&f);
  
  config->sensor(i).writeHeader(str);
  af->channel(i).writeBytes(str);

  f.close();

  return true;
}



void WLTool::helpText(JTextStream& str) const {

  str << "\n";
  str << "archive_fp = <filepath>  file path or archive file to read\n";
  str << "sensor_setup = raw | fsf | fsf_no_hocr | custom    sensor configuration, default fsf.\n\n";
  str << "'fsf' or 'fsf_no_hocr'\n";
  str << "----------------------\n";
  str << "chanX_sensor    = RawData | ACS | ECOBB3 | CTD | HOCR  sensor tyoe for channel X, where X is 1,2,3...9\n";
  str << "chanX_device_fp = <filepath>       device file for channel X, where X is 1,2,3...9\n\n";
  str << "operation = preview | extract | extract_and_merge\n";
  str << "channel_files_save_fp = <filepath>   prefix to save channel files to.\n.";
  str << "merge_file_channels = <index,index,...index>   channels to include in output merge file.\n";
  str << "\n";

  Tool::helpText(str);
}

