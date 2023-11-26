#include "types.h"
#include"defs.h"
#include "param.h"
#include "memlayout.h"
#include"mmu.h"
#include"spinlock.h"
#include"proc.h"

struct Block
{
    struct Block *next;
};
struct{
    struct spinlock Lock;
    struct Block *Free_MEM;

}Kernel_Memory;
void Kernel_FREE(char *p){
    //struct Block *r;
    if((uint)p %PGSIZE || (uint)p<KERNLINK || V2P(p)>=PHYSTOP)
        panic("Kernel FREE");


}
void Kernel_init(void){
    char* p;
    p=(char *)PGROUNDDOWN((uint)KERNLINK);
    for(;p+PGSIZE<=(char *)PHYSTOP;p+=PGSIZE)
        Kernel_FREE(p);
        
}
