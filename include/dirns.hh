#pragma once

#include "nstbl.hh"

u64 namehash(const strbuf<DIRSIZ>&);
class dirns : public nstbl<strbuf<DIRSIZ>, u32, namehash> {};
