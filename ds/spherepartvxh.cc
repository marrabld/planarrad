
// spherepartvxh.cc

#include "base/jgenericfactory.h"
#include "spherepartvxh.h"
#include "hemispherepartvxh.h"
#include "base/jlog.h"
#include "base/jerror.h"
#include "base/jparser.h"
//#include "direcadap.h"

using namespace jude::ds;



// static table list

SpherePartVxH::TableList SpherePartVxH::table_list;


// table list destructor - clears contents on program exit

SpherePartVxH::TableList::~TableList() {
  SPartTable* t = first();
  while (t) {
    delete t;
    t = next();
  }
}



// SpherePartVxH::(constructor)

SpherePartVxH::SpherePartVxH(const DirecStruct& ds) {

  const SpherePartVxH* ptr = dynamic_cast<const SpherePartVxH*>(&ds);
  if (!ptr) { jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " incompatible directional structure\n"; internalError(); }
  tab = ptr->tab;
}



// SpherePartVxH::(constructor)

SpherePartVxH::SpherePartVxH(JDataStream& str) {
  read(str);
}



// SpherePartVxH::(constructor)

//SpherePartVxH::SpherePartVxH(JTextStream& str) {
  //int vn,hn;
  //str >> vn;
  //str >> hn;
  //findOrAddTable(vn,hn);
//}




// SpherePartVxH::(constructor)
// build a sphere partition based on a hemisphere partition

SpherePartVxH::SpherePartVxH(const HemiSpherePartVxH& ds) {

  int vn = ds.vertSize()<<1;

  if (ds.isRegular()) { findOrAddTable(vn, ds.horizSize(), 0); return; }

  std::vector<double> tp(vn + 3);

  for (int i=0;i<=ds.vertSize();i++) {
    tp[i] = ds.thetaPointsDeg()[i];
    tp[vn+2-i] = 180 - ds.thetaPointsDeg()[i];
  }

  tp[ds.vertSize()+1] = 90;

  findOrAddTable(vn, ds.horizSize(), &tp);
}



SpherePartVxH::~SpherePartVxH() {

}



// SpherePartVxH::findOrAddTable() [private]
// finds a tabulated data structure for a sphere partition of size (vn,hn) 
// from the static list of all created so far
// if can't find one then creates a new one and adds to the list

void SpherePartVxH::findOrAddTable(int vn, int hn, const std::vector<double>* tp_deg) {
 
  if (tp_deg) if (((int)tp_deg->size()) != vn+3) {
    jlog::es << "Internal error " << __FILE__ << ":" << __LINE__ << " wrong number of theta points (" << (int) tp_deg->size() << " should be " << vn+3 << ")\n";
    internalError();   
  }

    // go through table list
  tab = table_list.first();

  while (tab) {
      // if found one of the right size return it
    if (tab->vertSize() == vn && tab->horizSize() == hn) {
      if (tp_deg) { if (tab->thetaPointsDeg() == *tp_deg) return; }
      else if (tab->isRegular()) return;
    }
       // else go onto next table
    tab = table_list.next();
  }
    // not found one of right size so create new table
  tab = (tp_deg) ? new SPartTable(vn, hn, *tp_deg) : new SPartTable(vn, hn);
    // append to list
  table_list.append(tab);

    // tab is now in table list so should be able to use this type to build adap quad structure
    // however note adap data can only be used after this is created
  //tab->buildAdapTable(*this);
}



// SpherePartVxH::indexIsOnTop()

//bool SpherePartVxH::indexIsOnTop(int i) const {
//  return i < (units() >> 1); 
//}



// SpherePartVxH::undersideHemiDSIndex()
// returns the equivalent index for a HemiPartiton structure corresponding to an index
// on the undeside of a SpherePartition
// Note the general rule when flipping sides is that the structure is rotated on the x-axis
// this means that the quad seams are contiguous

//int SpherePartVxH::undersideHemiDSIndex(int i) const {
// return units() - 1 - i;
//}



// SpherePartVxH::isEqual()

bool SpherePartVxH::isSameType(const DirecStruct& ds) const {
  return (dynamic_cast<const SpherePartVxH*>(&ds) != 0);
} 



// SpherePartVxH::isEqual()

bool SpherePartVxH::isEqual(const DirecStruct& ds) const {
  const SpherePartVxH* ptr = dynamic_cast<const SpherePartVxH*>(&ds);
  if (!ptr) return false;
  if (vertSize()!=ptr->vertSize() || horizSize()!=ptr->horizSize()) return false;
  if (isRegular() && ptr->isRegular()) return true;
  return (thetaPointsDeg() == ptr->thetaPointsDeg());
} 



// SpherePartVxH::read()

void SpherePartVxH::read(JDataStream& str) {

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



// SpherePartVxH::write()

void SpherePartVxH::write(JDataStream& str) const {

  str << vertSize();
  str << horizSize();
 
  str << isRegular();
  if (!isRegular()) str << thetaPointsDeg();
}



/*! \brief Reads the directional structure from a text stream. 

/todo Currently not implemented. Remove or implement.
*/

//void SpherePartVxH::read(JTextStream& str) {
  //int vn,hn;
  //str >> vn;
  //str >> hn;
  //tab = findOrAddTable(vn,hn);
//}



/*! \brief Writes the directional structure to a text stream. 

/todo Currently not implemented. Remove or implement.
*/

//void SpherePartVxH::write(JTextStream& str) const {
  //str << vertSize();
  //str << horizSize();
//}



// SpherePartVxH::write()
/*
void SpherePartVxH::writeToDataStream(JDataStream& str) const {

  str << vertSize();
  str << horizSize();
 
  str << isRegular();
  if (!isRegular()) str << thetaPointsDeg();
}
*/


// SpherePartVxH::read()
/*
void SpherePartVxH::readFromDataStream(JDataStream& str) {

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

*/


/*! \fn SpherePartVxH::SpherePartVxH()
\brief Default constructor is a 4 rows x 8 columns regular partition. */



/*! \fn SpherePartVxH::SpherePartVxH(int vn, int hn, const std::vector<double>* tp=0)
\brief Construct a \a vn rows by \a hn columns spherical partition, with optional specification of zenith angle boundaries \a tp.

If \a tp is null, then a regular speherical partition is created where the zenith intrervals are all equal and equal to half of the end cap zenith angle boundary.
If \a tp is not null it must point to a std::vector of size \a vn + 3, and specifies the zenith angle boundaries in degrees starting at zero and uncreasing to a final value of 180.
*/
 


/*! \fn SpherePartVxH::SpherePartVxH(int vn, int hn, const std::vector<double>& tp)
\brief Construct a \a vn rows by \a hn columns spherical partition, with specification of zenith angle boundaries \a tp.

Note \a tp must be of size \a vn + 3, and specifies the zenith angle boundaries in degrees starting at zero and uncreasing to a final value of 180.
*/

