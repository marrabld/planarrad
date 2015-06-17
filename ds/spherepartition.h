
// spherepartition.h

#ifndef _JUDE_DS_SPHEREPARTITION_H
#define _JUDE_DS_SPHEREPARTITION_H

#include "direcstruct.h"
#include <vector>

namespace jude {
namespace ds {



// SpherePartition

class SpherePartition : public DirecStruct {

public:

  bool isSpherePartition() const { return true; }
  bool isCubePartition() const { return false; }

  static bool isDFTCapable() { return true; }
 
  virtual int vertSize() const = 0;
  virtual int horizSize() const = 0;

  virtual bool isRegular() const = 0;
  virtual const std::vector<double>& thetaPointsDeg() const = 0;
  virtual const std::vector<double>& thetaPointsRad() const = 0;
 
  virtual int positionToIndex(int vp, int hp) const = 0;

    // depreceated?
  virtual int positionToIndexDFT(int vp, int hp) const = 0;

  void displayTableValues(JTextStream& str, int code, const JString& major_sep) const;
  void displayTableValues(JTextStream& str, const QuadDataSource& qds, const JString& major_sep, const JString& sub_sep) const;

  void readTableValues(const JTextTable& ttab, int start_row, int start_col, const QuadDataWrite& qdw) const;

  void display(JTextStream& str, int code, const JString& major_sep) const;
};



} } // namespace jude::ds

#endif // _JUDE_DS_SPHEREPARTITION_H

