
// jmagicnum.h

#ifndef _JUDE_BASE_MAGICNUM_H
#define _JUDE_BASE_MAGICNUM_H



namespace jude {
namespace base {


// this is just an arbitrary 32 bit number which is written at the start and end of all data files
// its purpose is to help to quickly discount input files which are not correct or are corrupted
// should be saved as uns32

#define JUDE_MAGICNUM 0x94F7A891



} } // namespace jude::base

#endif // _JUDE_BASE_MAGICNUM_H

