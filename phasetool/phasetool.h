
// phasetool.h

#ifndef _JUDETOOLS_PHASETOOL_H
#define _JUDETOOLS_PHASETOOL_H

#include "tool/tool.h"
#include "rad/raddirecdirecimp.h"
#include "rad/sbanddata.h"
#include "rad/phasefunc.h"


//#include "spectraldata_multi.h"
//#include "reflecimp.h"

//class Parser;
//class PhaseFunc;

using namespace jude::tool;
using namespace jude::rad;
using namespace jude::ds;



class PhaseTool : public Tool {
 
  BandSpec* master_bs;
  int band_count;

  DirecSpec* master_ds_spec;

  RadDirecDirecImp<SBandData>* phase_func_tab;

  double sub_quad_sa;

  std::vector<double> wl;

  bool processing_canceled;

  void init();

  bool loadFile(const JParser& prm);
  bool getBandMode(JString& mode, const FileSetDesc& fsd, const JParser& prm);
  bool integratePhaseFunc(const PhaseFunc& func);

public:

  PhaseTool();
  ~PhaseTool();

  const JString name() const { return "phasetool"; }

  bool run(const JParser& prm);

  void helpText(JTextStream& str) const;
};



#endif // _JUDETOOLS_PHASETOOL_H

