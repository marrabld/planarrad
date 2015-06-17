
#include "hemispherepartvxh.h"
#include "spherepartvxh.h"
#include "hsparttable.h"
#include "base/jlog.h"
#include "base/jerror.h"
#include "base/jparser.h"
//#include "direcadap.h"

using namespace jude::ds;



// static table list

HemiSpherePartVxH::TableList HemiSpherePartVxH::table_list;


// table list destructor - clears contents on program exit

HemiSpherePartVxH::TableList::~TableList() {
  HSPartTable* t = first();
  while (t) {
    delete t;
    t = next();
  }
}



// HemiSpherePartVxH::(constructor)

HemiSpherePartVxH::HemiSpherePartVxH(const DirecStruct& ds) {

  const HemiSpherePartVxH* ptr = dynamic_cast<const HemiSpherePartVxH*>(&ds);
  if (!ptr) { jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " incompatible directional structure\n"; internalError(); }
  tab = ptr->tab;
}



// HemiSpherePartVxH::(constructor)

HemiSpherePartVxH::HemiSpherePartVxH(JDataStream& str) {
  read(str);
}
  


// HemiSpherePartVxH::(constructor)

HemiSpherePartVxH::HemiSpherePartVxH(JTextStream& str) {
  //int vn,hn;
  //str >> vn;
  //str >> hn;
  //tab = findOrAddTable(vn,hn);
}



// HemiSpherePartVxH::(constructor)
 
HemiSpherePartVxH::HemiSpherePartVxH(const SpherePartVxH& ds) {

  int vn = ds.vertSize()>>1;

  if (ds.isRegular()) { findOrAddTable(vn, ds.horizSize(), 0); return; }

  std::vector<double> tp(vn + 2);

  for (int i=0;i<=vn;i++) tp[i] = ds.thetaPointsDeg()[i];
  tp[vn+1] = 90;

  findOrAddTable(vn, ds.horizSize(), &tp);
}



// HemiSpherePartVxH::findOrAddTable() [private]
// finds a tabulated data structure for a hemi cube partition of size (n) 
// from the static list of all created so far
// if can't find one then creates a new one and adds to the list

void HemiSpherePartVxH::findOrAddTable(int vn, int hn, const std::vector<double>* tp_deg) {

  if (tp_deg) if (((int)tp_deg->size()) != vn+2) {
    jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " wrong number of theta points (" << (int) tp_deg->size() << " should be " << vn+2 << ")\n";
    internalError();   
  }

    // go througn table list
  tab = table_list.first();

  while (tab) {
      // if found one of the right size return it
    if (tab->vertSize() == vn && tab->horizSize() == hn) {
        // if theta points match
      if (tp_deg) { if (tab->thetaPointsDeg() == *tp_deg) return; }
      else if (tab->isRegular()) return;
    }
       // else go onto next table
    tab = table_list.next();
  }
    // not found one of right size so create new table
  tab = (tp_deg) ? new HSPartTable(vn, hn, *tp_deg) : new HSPartTable(vn, hn);
    // append to list
  table_list.append(tab);

    // tab is now in table list so should be able to use this type to build adap quad structure
    // however note adap data can only be used after this is created
  //tab->buildAdapTable(*this);
}



const SpherePartVxH* HemiSpherePartVxH::createFull() const { 
  return new SpherePartVxH(*this); 
}



// HemiSpherePartVxH::isSameType()

bool HemiSpherePartVxH::isSameType(const DirecStruct& ds) const {
  return (dynamic_cast<const HemiSpherePartVxH*>(&ds) != 0);
} 



// HemiSpherePartVxH::isEqual()

bool HemiSpherePartVxH::isEqual(const DirecStruct& ds) const {
  const HemiSpherePartVxH* ptr = dynamic_cast<const HemiSpherePartVxH*>(&ds);
  if (!ptr) return false;
  if (vertSize()!=ptr->vertSize() || horizSize()!=ptr->horizSize()) return false;
  if (isRegular() && ptr->isRegular()) return true;
  return (thetaPointsDeg() == ptr->thetaPointsDeg());
} 



// HemiSpherePartVxH::read()

void HemiSpherePartVxH::read(JDataStream& str) {

  int vn,hn;
  str >> vn;
  str >> hn;

  bool is_reg;
  str >> is_reg;

  if (is_reg) return findOrAddTable(vn,hn,0);

  std::vector<double> tp_deg;
  str >> tp_deg;
 
  findOrAddTable(vn, hn, &tp_deg);
}



// HemiSpherePartVxH::write()

void HemiSpherePartVxH::write(JDataStream& str) const {

  str << vertSize();
  str << horizSize();

  str << isRegular();
  if (!isRegular()) str << thetaPointsDeg();
}



// HemiSpherePartVxH::read()

//void HemiSpherePartVxH::read(JTextStream& str) {
  //int vn,hn;
  //str >> vn;
  //str >> hn;
  //tab = findOrAddTable(vn,hn);
//}



// HemiSpherePartVxH::write()

//void HemiSpherePartVxH::write(JTextStream& str) const {
// str << vertSize();
// str << horizSize();
//}



// HemiSpherePartVxH::write()
/*
void HemiSpherePartVxH::writeToDataStream(JDataStream& str) const {

  str << vertSize();
  str << horizSize();
 
  str << isRegular();
  if (!isRegular()) str << thetaPointsDeg();
}



// HemiSpherePartVxH::read()

void HemiSpherePartVxH::readFromDataStream(JDataStream& str) {

  int vn,hn;
  str >> vn;
  str >> hn;

  bool is_reg;
  str >> is_reg;

  if (is_reg) return findOrAddTable(vn,hn,0);

  std::vector<double> tp_deg;
  str >> tp_deg;

  findOrAddTable(vn, hn, &tp_deg);
}



// HemiSpherePartVxH::collectInfo()

void HemiSpherePartVxH::collectInfo(JParser& prm, const JString& prefix) const {
  prm.setLabelAndValue(prefix+"partition", "hemisphere");
  prm.setLabelAndValue(prefix+"hvn", JString::number(vertSize()));
  prm.setLabelAndValue(prefix+"hn", JString::number(horizSize()));
}
*/

