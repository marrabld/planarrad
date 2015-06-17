
// wltool.h

#ifndef _JUDETOOLS_WLTOOL_H
#define _JUDETOOLS_WLTOOL_H

#include "tool/tool.h"
#include "wl/archivefile.h"
#include "wl/sensorconfig.h"

using namespace jude::tool;
using namespace jude::wl;


class WLTool : public Tool {

  SensorConfig* config;

  ArchiveFile* af;

  bool sensor_setup_ok;
  bool records_extracted_ok;

  int loading_processed_channel;
 
  void init();

  bool saveChannelFiles(const JString& base_fp);

public:

  WLTool();
  ~WLTool();

  const JString name() const { return "wltool"; }

  bool run(const JParser& prm);

  const SensorConfig& sensorConfig() const { return *config; }
  bool buildSensorConfig(const JParser& prm);
  void setSensorConfig(SensorConfig* new_config) { delete config; config = new_config; }
 
  ArchiveFile* archiveFile() { return af; }
  const ArchiveFile* archiveFile() const { return af; }
  void deleteArchiveFile();

  double pctComplete() const;

  void stopProcessing();

  bool loadArchiveFile(const JString& archive_fp, bool preview_only, const JParser& prm);

  //bool sensorSetupOK() const { return sensor_setup_ok; }
  bool recordsExtractedOK() const { return records_extracted_ok; }

  bool saveChannelFile(const JString& fp, int i);

  const JString channelFilepath(const JString& base_fp, int i);

  bool doMerge(const JParser& prm);
  bool saveMergedFile(const JString& fp);

  bool mergeDone() { return (af) ? af->channelMerge().recordCount() > 0 : false; }
  void helpText(JTextStream& str) const;
};



#endif // _JUDETOOLS_WLTOOL_H

