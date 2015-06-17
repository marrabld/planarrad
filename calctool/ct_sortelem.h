
// ct_sortelem.h

#ifndef _JUDETOOLS_CT_SORTELEM_H
#define _JUDETOOLS_CT_SORTELEM_H



class CT_SortElem {

  double value;
  int index_val;

public:

  CT_SortElem(double v, int i) : 
    value(v), 
    index_val(i) { }

  int index() const { return index_val; }

  static bool compareForSort(const CT_SortElem& se1, const CT_SortElem& se2) { return se1.value > se2.value; }
};



#endif // _JUDETOOLS_CT_SORTELEM_H

