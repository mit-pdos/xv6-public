#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"

#define SEMNUM 32

struct semaphore {
  int value;
  int active;
  struct spinlock lock;
} sem[SEMNUM];

//initiallize the semophore table
void
seminit(void)
{
  for(int i = 0; i < SEMNUM; ++i) {
    initlock(&sem[i].lock, "semaphore");
    sem[i].active = sem[i].value = 0;
  }
}

int
sem_init(int num, int max)
{
  //check if the entry is valid
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  // check if the entry is actived
  if(sem[num].active != 0) return -1;
  sem[num].value = max;
  sem[num].active = 1;
  release(&sem[num].lock);

  return 0;
}

int
sem_destroy(int num)
{
  // check if the entry is valid
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  // check if the entry is actived
  if(sem[num].active != 1) return -1;
  sem[num].active = 0;
  release(&sem[num].lock);

  return 0;
}

int
sem_wait(int num, int count)
{
  // check if  valid
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  // check if active
  if(!sem[num].active) return -1;
  // if there is no enough lock, sleep
  while(sem[num].value <= count - 1) {
    sleep(&sem[num], &sem[num].lock);
  }
  sem[num].value -= count;
  release(&sem[num].lock);

  return 0;
}

int
sem_signal(int num, int count)
{
  // check if the entry is valid
  if(num < 0 || num >= SEMNUM) return -1;

  acquire(&sem[num].lock);
  // check if the entry is actived
  if(!sem[num].active) return -1;
  // the thread exit the critical section, return a lock
  sem[num].value += count;
  // if there is any available lock, call wakeup
  if(sem[num].value > 0){
    wakeup(&sem[num]);
  }
  release(&sem[num].lock);

  return 0;
}
