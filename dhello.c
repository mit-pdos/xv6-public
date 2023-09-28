#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

int
helloread(struct inode *ip, char *dst, int n) {
    static const char *s_charactersToRead = "Hello, World!\n";
    static const int s_lenCharactersToReadcharactersToRead = 15;

    if(n > s_lenCharactersToReadcharactersToRead) {
        n = s_lenCharactersToReadcharactersToRead;
    }
    for(int i = 0; i < n; ++i) {
        dst[i] = s_charactersToRead[i];
    }
    return n;
}

int
hellowrite(struct inode *ip, char *src, int n) {
    return n;
}


void
helloinit(void)
{
  //initlock(&cons.lock, "hello");

  devsw[HELLO].write = hellowrite;
  devsw[HELLO].read = helloread;
  //cons.locking = 1;
}

