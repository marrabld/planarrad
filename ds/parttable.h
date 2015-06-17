
// parttable.h

#ifndef _JUDE_DS_PARTTABLE_H_JDH
#define _JUDE_DS_PARTTABLE_H_JDH

//#include "ds/da_nodeiter.h"
#include "base/jlist.h"
#include <stdio.h>

using namespace jude::base;

namespace jude {
namespace ds {



/*! \brief Base class for directional spherical and hemispherical partition tables. */

class PartTable {

public:

  template <typename Q, typename DS>
  void buildAdapTable(JList<Q>& all_quads_list, Q* quad_array, Q**& adap_array, const DS& ds);
};



// PartTable::buildAdapTable()

template <typename Q, typename DS>
void PartTable::buildAdapTable(JList<Q>& all_quads_list, Q* quad_array, Q**& adap_array, const DS& ds) {

  /*

    // list all quads just so they can be deleted on destruction
  all_quads_list.setAutoDelete(true);

    // allocate table of pointers
  adap_array = new Q*[(ds.adapMaxDepth()+1) * ds.units()];
 
    // for testing
  for (int i=0;i<(ds.adapMaxDepth()+1) * ds.units();i++) adap_array[i] = 0;

    // initialise bottom level quads
  int base = ds.adapMaxDepth() * ds.units();
  for (int i=0;i<ds.units();i++) adap_array[base + i] = &quad_array[i];

    // non adaptive quads must be copied in all levels
  for (int i=0;i<ds.nonAdapIndexNum();i++) {
    for (int d=0;d<ds.adapMaxDepth();d++) {
      adap_array[d * ds.units() + ds.nonAdapIndex(i)] = &quad_array[ds.nonAdapIndex(i)];
    }
  }

    // loop through all nodes and leaves of directional structure
  DA_NodeIter<DS> iter(ds);
  bool f = iter.first();
  while (f) {

      // only do non-leaf nodes
    if (iter.depth() < ds.adapMaxDepth()) {

      int base = (iter.depth()+1) * ds.units();

        // get the four child quads (or null pointers if less)
      Q* q0 = iter.child0Valid() ? adap_array[base + iter.child0BaseIndex()] : 0; 
      Q* q1 = iter.child1Valid() ? adap_array[base + iter.child1BaseIndex()] : 0; 
      Q* q2 = iter.child2Valid() ? adap_array[base + iter.child2BaseIndex()] : 0; 
      Q* q3 = iter.child3Valid() ? adap_array[base + iter.child3BaseIndex()] : 0; 

        // create a combined quad
      Q* q = new Q(q0,q1,q2,q3);
      all_quads_list.append(q);

      base = iter.depth() * ds.units();

        // loop through all the positions that this quad represents
      for (int yp=iter.yPos(); yp<iter.yPos()+iter.equivYQuads(); yp++) {
	for (int xp=iter.xPos(); xp<iter.xPos()+iter.equivXQuads(); xp++) {
	    // only for valid positions
	  if (iter.adapBase().validPos(xp, yp)) {
	      // write pointer in table
	    adap_array[base + iter.adapBase().posToIndex(xp,yp)] = q;
	  }
	}
      }
    }

    //printf("d %d x %d y %d index %d\n",iter.depth(),iter.xPos(),iter.yPos(),iter.baseIndex());
    f = iter.next();
  }

  return;

    // for testing
  for (int i=0;i<ds.units();i++) {
    printf("%d: ",i);
    for (int d=0;d<=ds.adapMaxDepth();d++) {
      Q* q = adap_array[d*ds.units() + i];
      printf("%f ",q->solidAngle()); 
      //printf("%p ",q); 
    }
    printf("\n");
  }

  */

}



} } // namespace jude::ds

#endif // _JUDE_DS_PARTTABLE_H

