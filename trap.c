#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

void
resetaccessed()
{
  pte_t *page;
  int i, j;

  for(i = 0; i < NPDENTRIES; i++){
    for(j = 0; j < NPTENTRIES; j++){
      page = FINDPAGE(proc->pgdir, i, j);
      *page &= ~PTE_A;
    }
  }
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(proc->killed)
      exit();
    proc->tf = tf;
    syscall();
    if(proc->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpunum() == 0){
      acquire(&tickslock);
      ticks++;
      if(SELECTION == NFU)
        resetaccessed();
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpunum(), tf->cs, tf->eip);
    lapiceoi();
    break;
  case T_PGFLT:
    if(SELECTION == NONE)
      goto do_not_handle;

    pte_t *page, *temp, *pagetime, *temptime;
    int i, j, numpages;

    numpages = 0;
    temp = FINDPAGE(proc->pgdir, 0, 0);
    temptime = FINDPAGE(proc->pgdirtimes, 0, 0);

    for(i = 0; i < NPDENTRIES; i++){
      for(j = 0; j < NPTENTRIES; j++){
        page = FINDPAGE(proc->pgdir, i, j);
        pagetime = FINDPAGE(proc->pgdirtimes, i, j);

        if((*page & PTE_P) && !(*page & PTE_PG)){
          numpages++;

          switch (SELECTION){
            case NFU:
              // Keep using temp by default, unless page has not been recently accessed
              temp = (*page & PTE_A) ? temp : page;
              break;
            case FIFO:
              // Keep using temp by default, unless page was created earlier
              if(*pagetime < *temptime){
                temp = page;
                temptime = pagetime; 
              }
              break;
            default:
              panic("SELECTION not set to a valid option");
          }
        }
      }
    }

    if(numpages == MAX_PSYC_PAGES){
      storepage(proc->pid, temp);
      // @TODO: get address that caused the page fault
      loadpage(proc->pid, temp);
    }

    tf->eip--;
    break;

  //PAGEBREAK: 13
do_not_handle:
  default:
    if(proc == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpunum(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            proc->pid, proc->name, tf->trapno, tf->err, cpunum(), tf->eip,
            rcr2());
    proc->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(proc && proc->state == RUNNING && tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(proc && proc->killed && (tf->cs&3) == DPL_USER)
    exit();
}
