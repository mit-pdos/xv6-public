// #include "types.h"
// // #include "defs.h"
// #include "param.h"
// #include "memlayout.h"
// #include "mmu.h"
// #include "x86.h"
// #include "proc.h"
// #include "spinlock.h"
// #include "user.h"
// struct {
//   struct spinlock lock;
//   struct proc proc[NPROC];
// } ptable;
//
// // static struct proc *initproc;
//
// int nextpid = 1;
// extern void forkret(void);
// extern void trapret(void);
//
//
// int main(){
//   struct proc *p;
//   // acquire(&ptable.lock);
//   printf(1,"PID            Status\n");
//   for(p = ptable.proc; p!=0 && p < &ptable.proc[NPROC]; p++){
//     printf(1,"%d             ",p->pid);
//     if(p->state == RUNNABLE){
//       printf(1,"RUNNABLE\n");
//       continue;
//       }
//     else if(p->state == RUNNING){
//       printf(1,"RUNNING\n");
//       continue;
//     }
//     else{
//       printf(1,"Don't Know\n");
//     }
//     // exit();
// }
// // release(&ptable.lock);
//   return 0;
// }
