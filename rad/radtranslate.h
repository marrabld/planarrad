
// radtranslate.h

#ifndef _JUDE_RAD_RADTRANSLATE_H
#define _JUDE_RAD_RADTRANSLATE_H

#include "raddirecdirectable.h"
#include "raddirectable.h"

using namespace jude::rad;



template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::translateInToOut(const RadDirecTable<A,DS>& rdin, RadDirecTable<A,DS>& rdout) const {
  ddt.translateInToOut(rdin.tableData(), rdout.tableData());
}



template <typename A, typename DS>
void RadDirecDirecTable<A,DS>::translateInToOut(const RadDirecImp<A>& rdin, RadDirecImp<A>& rdout) const {

  const RadDirecTable<A,DS>* ptr_in = dynamic_cast< const RadDirecTable<A,DS>* >(&rdin);
  RadDirecTable<A,DS>* ptr_out = dynamic_cast< RadDirecTable<A,DS>* >(&rdout);
  if (ptr_in && ptr_out) return translateInToOut(*ptr_in, *ptr_out);
  jlog::es << "RadDirecDirecTable:: translateInToOut() - unsupported input type(s).\n";
  internalError();
}



#endif // _JUDE_RAD_RADTRANSLATE_H

