
#ifndef _JUDE_WL_TEXT_H
#define _JUDE_WL_TEXT_H

#include "record.h"
#include "base/jlist.h"
#include "wl/blockstream.h"

namespace jude {
namespace wl {



class Record_TEXT : public Record {

protected:

  class TextItem {
    BlockStream bs;
    int len;
  public:
    TextItem(const BlockStream& bs) : bs(bs), len(0) { }
    int length() const { return len; }
    void setLength(int v) { len = v; }
    const BlockStream& streamStart() const { return bs; }
  };


  JList<TextItem> item_list;

  void deleteAllItems();
  bool decompose(int& bytes_read);

  virtual bool buildRecord(int& final_pos) = 0;

public:

  Record_TEXT(const Sensor* s);
  ~Record_TEXT();

  virtual void appendBytes(const uns8*& ptr, int& bytes_left);

  void writeBytes(JDataStream& str) const;

  void writeProcessed(JTextStream& str) const;
  
  int itemCount() const { return item_list.count(); }
  const JString itemToString(int index) const;

  const JString processed(int i) const;

  const char* textDisplayLine(int i) const;

  void display() const;
};



} } // namespace jude::wl

#endif // _JUDE_WL_TEXT_H

