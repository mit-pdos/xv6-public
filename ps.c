#include "types.h"
#include "user.h"
#include "pstat.h"

int main(int argc, char* argv[]){
  struct pstat ps;
  while(1){
    getpinfo(&ps);
    printf(1, "\nPID\t|\tUSED?\t|\tTickets\t|\tticks\n");
    for (int i = 0; i < sizeof(ps.pid)/sizeof(ps.pid[0]); i++)
    {
      if(ps.pid[i])
        printf(1, "%d\t|\t%d\t|\t%d\t|\t%d\n", ps.pid[i], ps.inuse[i], ps.tickets[i], ps.ticks[i]);
    }
    sleep(200);
  }
  exit();
  return 0;
}