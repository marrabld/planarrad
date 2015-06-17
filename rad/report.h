
// report.h

#ifndef _JUDE_RAD_REPORT_H
#define _JUDE_RAD_REPORT_H

#include "rad/raddirecimp.h"
#include "rad/bandspec.h"
#include "base/jlog.h"
#include "base/jerror.h"
#include "base/jfile.h"

using namespace jude::ds;

namespace jude {
namespace rad {


class Report {

  JString major_sep;
  JString sub_sep;

  JFile* file;
  JTextStream str;

public:

  Report();
  ~Report();

  bool openFile(const JString& filepath);
  void closeFile();

  const JString& majorSep() const { return major_sep; }
  const JString& subSep() const { return sub_sep; }

  JTextStream& stream() { return str; }

  template <typename SD>
  void writeBandHeader(const JString& text, const JString& col1, const JString& col2, int band_count);

  template <typename SD>
  void writeSpectralData(const JString& code, double depth, const SD& sd);

  template <typename SD>
  void writeSpectralData(const JString& code, const SD& sd);

  template <typename SD>
  void writeTableData(const JString& code, const RadDirecImp<SD>& rd);

  //template <typename SD>
  //void writePlanarIrrad(const JString& code, double depth, const RadDirecImp<SD>& rd);
};



template <typename SD>
  void Report::writeBandHeader(const JString& text,  const JString& col1, const JString& col2, int band_count) {
  str << "\n# " << text << "\n";
  str << "# " << col1 << major_sep << col2;
  for (int i=0; i<band_count; i++) { str << major_sep << SD::policy_type::reportBandHeader(sub_sep, i); }
  str << "\n";
}



template <typename SD>
void Report::writeSpectralData(const JString& code, const SD& sd) {
  str << code << major_sep << "-" << major_sep;
  sd.write(str, major_sep, sub_sep);
  str << "\n";
}



template <typename SD>
void Report::writeSpectralData(const JString& code, double depth, const SD& sd) {
  str << code << major_sep << depth << major_sep;
  sd.write(str, major_sep, sub_sep);
  str << "\n";
}



template <typename SD>
void Report::writeTableData(const JString& code, const RadDirecImp<SD>& rd) {

  for (int band_index=0; band_index<rd.bandCount(); band_index++) {

    str << "\n" << code << " band " << band_index+1 << "\n";

    RadDirecQDS<SD> qds(rd, band_index);
    rd.direcStruct().displayTableValues(str, qds, major_sep, sub_sep);
  }
}



} } // namespace jude::rad

#endif // _JUDE_RAD_REPORT_H

