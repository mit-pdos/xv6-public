#pragma once

#include "atomic.hh"

struct pgmap {
  std::atomic<pme_t> e[PGSIZE / sizeof(pme_t)];
};

extern pgmap kpml4;

void            freevm(pgmap *pml4);
pgmap*          setupkvm(void);
int             setupkshared(pgmap *pml4, char *kshared);
std::atomic<pme_t>* walkpgdir(pgmap *pml4, u64, int);
void            tlbflush(void);

template<class CB>
void
updatepages(pgmap *pml4, u64 begin, u64 end, CB cb)
{
  for (u64 a = PGROUNDDOWN(begin); a != PGROUNDUP(end); a += PGSIZE) {
    std::atomic<pme_t> *pte = walkpgdir(pml4, a, 1);
    cb(pte);
  }
}
