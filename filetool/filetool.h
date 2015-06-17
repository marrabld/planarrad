
// filetool.h

#ifndef _RADTOOLS_FILETOOL_H
#define _RADTOOLS_FILETOOL_H

#include "tool/tool.h"
#include "rad/raddirecimp.h"
#include "rad/raddirecdirecimp.h"
#include "rad/spectralimage.h"

using namespace jude::tool;
using namespace jude::rad;



class FileTool : public Tool {

  int ds_table_code;

  void init();

  void doRadDirec(RadDirecImp<SBandData>* rd, const FileInfo& fi, const JParser& prm);
  void doRadDirecDirec(RadDirecDirecImp<SBandData>* rdd, const FileInfo& fi, const JParser& prm);
  void doSpectralImage(SpectralImage<SBandData>* sif, const FileInfo& fi, const JParser& prm);

  void convertSifToRGB(SpectralImage<SBandData>* sif, const FileInfo& fi, const JParser& prm);
  void convertSifToScale(SpectralImage<SBandData>* sif, const FileInfo& fi, const JParser& prm);

  bool createRadDirec(const JParser& prm);

  bool writeAngularDataFile(const JParser& prm, const RadDirecImp<SBandData>& rd);
  bool writeAngularDataFile(const JParser& prm, const RadDirecDirecImp<SBandData>& rdd);

  template <typename T>
  void displayInfo(const T& obj, const FileInfo& fi, const JParser& prm);

public:

  FileTool();
  ~FileTool();

  const JString name() const { return "filetool"; }

  bool run(const JParser& prm);

  void helpText(JTextStream& str) const;
};



#endif // _RADTOOLS_FILETOOL_H

