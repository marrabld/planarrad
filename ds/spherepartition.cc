
// spherepartition.cc

#include "ds/spherepartition.h"
#include "ds/dsquad.h"
#include "ds/quaddatasource.h"
#include "base/jlog.h"

using namespace jude::ds;



// SpherePartition::display()

void SpherePartition::display(JTextStream& str, int code, const JString& major_sep) const {
 
  str.setScientific(JUDE_DEFAULT_OUTPUT_PREC);
  
  str << (isFull() ? "Sphere partition (SpherePartitionVxH) " : "Hemi-sphere partition (HemiSpherePartVxH) ");
  str << vertSize() << " x " << horizSize() << (isRegular() ? " Regular" : " Non-Regular") << "\n";
  str << "Quads: " << units() << "\n";

  str << "Vertical theta boundaries:\n";
  
  for (unsigned int i=0; i<thetaPointsDeg().size()-1; i++) str << thetaPointsDeg()[i] << ", ";
  str << thetaPointsDeg()[thetaPointsDeg().size()-1] << " (degrees)\n";

  for (unsigned int i=0; i<thetaPointsRad().size()-1; i++) str << thetaPointsRad()[i] << ", ";
  str << thetaPointsRad()[thetaPointsRad().size()-1] << " (radians)\n";

    // range of solid angles
  double max_sa = solidAngle(1);
  double min_sa = solidAngle(1);

  int u_max = isFull() ? units()-1 : units();
  for (int i=2; i<u_max; i++) {
    double sa = solidAngle(i);
    if (sa > max_sa) max_sa = sa;
    if (sa < min_sa) min_sa = sa;
  }
  str << "Solid angle range: " << min_sa << " to " << max_sa << " (sr) max ratio: " << max_sa/min_sa << "\n";

  if (code & DsQuad::SA) displayTableValues(str, DsQuad::SA, major_sep);
  if (code & DsQuad::SACT) displayTableValues(str, DsQuad::SACT, major_sep);
  if (code & DsQuad::CT) displayTableValues(str, DsQuad::CT, major_sep);
  if (code & DsQuad::INDEX) displayTableValues(str, DsQuad::INDEX, major_sep);
  if (code & DsQuad::POINTDIREC) displayTableValues(str, DsQuad::POINTDIREC, major_sep);
  if (code & DsQuad::MEANDIREC) displayTableValues(str, DsQuad::MEANDIREC, major_sep);
  if (code & DsQuad::MEANTHETA) displayTableValues(str, DsQuad::MEANTHETA, major_sep);
  if (code & DsQuad::MEANPHI) displayTableValues(str, DsQuad::MEANPHI, major_sep);
  if (code & DsQuad::POINTTHETA) displayTableValues(str, DsQuad::POINTTHETA, major_sep);
  if (code & DsQuad::POINTPHI) displayTableValues(str, DsQuad::POINTPHI, major_sep);
}



// SpherePartition::displayTableValues(int code) [private]

void SpherePartition::displayTableValues(JTextStream& str, int code, const JString& major_sep) const {

  switch (code) {
    case DsQuad::SA : str << "# Quad solid angles "; break;
    case DsQuad::SACT : str << "# Quad solid angle x abs cos(theta) "; break;
    case DsQuad::CT : str << "# Quad mean cos(theta) "; break;
    case DsQuad::INDEX : str << "# Quad index "; break;
    case DsQuad::POINTDIREC : str << "# Quad direction point vector "; break;
    case DsQuad::MEANDIREC : str << "# Quad solid angle mean point vector "; break;
    case DsQuad::POINTTHETA : str << "# Quad direction point theta "; break;
    case DsQuad::POINTPHI : str << "# Quad direction point phi "; break;
    case DsQuad::MEANTHETA : str << "# Quad solid angle mean point theta "; break;
    case DsQuad::MEANPHI : str << "# Quad solid angle mean point phi "; break;
  }
 
  str << "table (rows are horizontal, columns are vertical):\n";
 
    // top cap
  quadAtIndex(0)->display(str, 0, code);
  for (int hp=1; hp<horizSize(); hp++) str << major_sep << "-";
  str << "\n";
   
  for (int vp=1; vp<=vertSize(); vp++) {
    for (int hp=0; hp<horizSize(); hp++) {
      int index = positionToIndexDFT(vp,hp);
      quadAtIndex(index)->display(str, index, code);
      if (hp < horizSize()-1) str << major_sep;
    }
    str << "\n";
  }

    // bottom cap
  if (isFull()) {
    quadAtIndex(units()-1)->display(str, units()-1, code);
    for (int hp=1; hp<horizSize(); hp++) str << major_sep << "-";
    str << "\n";
  }

}



// SpherePartition::displayTableValues(int code) [private]

void SpherePartition::displayTableValues(JTextStream& str, const QuadDataSource& qds, const JString& major_sep, const JString& sub_sep) const {

  //str << qds.descriptionText() << " data table (rows are horizontal, columns are vertical):\n";
 
    // top cap
  qds.displayAtIndex(str, 0, sub_sep);
  for (int hp=1; hp<horizSize(); hp++) str << major_sep << "-";
  str << "\n";
   
  for (int vp=1; vp<=vertSize(); vp++) {
    for (int hp=0; hp<horizSize(); hp++) {
      int index = positionToIndexDFT(vp,hp);
      qds.displayAtIndex(str, index, sub_sep);
      if (hp < horizSize()-1) str << major_sep;
    }
    str << "\n";
  }

    // bottom cap
  if (isFull()) {
    qds.displayAtIndex(str, units()-1, sub_sep);
    for (int hp=1; hp<horizSize(); hp++) str << major_sep << "-";
    str << "\n";
  }
}



// SpherePartition::displayTableValues(int code) [private]

void SpherePartition::readTableValues(const JTextTable& ttab, int start_row, int start_col, const QuadDataWrite& qdw) const {

  //str << qds.descriptionText() << " data table (rows are horizontal, columns are vertical):\n";
 
    // top cap
  // qdw.setAtIndex(0, ttab.getText(start_row, start_col).toDouble());
  qdw.setAtIndex(0, ttab.getDoubleFromArray(start_row, start_col));
   
  for (int vp=1; vp<=vertSize(); vp++) {
    for (int hp=0; hp<horizSize(); hp++) {
      int index = positionToIndexDFT(vp,hp);
      //jlog::ls << start_row+vp << " " << start_col+hp << " " << ttab.getDoubleFromArray(start_row+vp, start_col+hp) << "\n";
      //qdw.setAtIndex(index, ttab.getText(start_row+vp, start_col+hp).toDouble());
      qdw.setAtIndex(index, ttab.getDoubleFromArray(start_row+vp, start_col+hp));
    }
  }

    // bottom cap
  if (isFull()) {
    //qdw.setAtIndex(units()-1, ttab.getText(start_row+1+vertSize(), start_col).toDouble());
    qdw.setAtIndex(units()-1, ttab.getDoubleFromArray(start_row+1+vertSize(), start_col));
  }
}


