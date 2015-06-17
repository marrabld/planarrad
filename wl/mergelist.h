
#ifndef _JUDE_WL_MERGELIST_H
#define _JUDE_WL_MERGELIST_H

#include "base/jlist.h"
#include "record_merge.h"

namespace jude {
namespace wl {

class Config;
class Channel;
class ArchiveFile;


class MergeList {

  JList<Record_MERGE> record_list;

  void deleteAll();

public:

  MergeList();
  ~MergeList();

  void build(const ArchiveFile& af, int merge_channel);

  const JList<Record_MERGE>& recordList() const { return record_list; }

  void display() const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_MERGELIST_H_JDH


