#include "types.h"
//#include "param.h"
//#include "mmu.h"
//#include "proc.h"
#include "user.h"

enum ticks_types {RETIME, RUTIME, STIME};

int main(int argc, char *argv[]) {
  int n;
  int pid, child_pid;
  int retime, rutime, stime;
  double cpu_ticks[3] = {0}, 
         s_ticks[3] = {0}, 
         io_ticks[3] = {0};

  if(argc != 2) {
    printf(1, "Numero de argumentos errados!\n");
    return -1;
  }

  n = atoi(argv[1]);
  
  for(int i = 0; i < 3*n; i++) {
    pid = fork();
    
    //Child process
    if(pid == 0) {
      child_pid = getpid();
      switch(child_pid % 3) {
        
        //CPU-Bound process
        case 0:
          for(int k = 0; k < 100; k++) {
            for(int l = 0; l < 1000000; l++) {
              //Do nothing
            }
          }
          break;
        
        //S-CPU process
        case 1:
          for(int k = 0; k < 100; k++) {
            for(int l = 0; l < 1000000; l++) {
              //Do nothing
            }
            yield();
          }
          break;

        //IO-Bound process
        case 2:
          for(int k = 0; k < 100; k++) {
            sleep(1);
          }
          break;
      }
      //Child process ends here
      exit();
    }

    //Parent process
    else {
      //Keeps creating children
      continue;
    }
  }

  for(int i = 0; i < 3*n; i++) {
  
    //Wait for a child to end
    pid = wait2(&retime, &rutime, &stime);
    switch(pid % 3) {
      case 0:
        printf(1, "Process pid: %i, type: CPU-Bound, retime: %i, rutime: %i, stime: %i", pid, retime, rutime, stime);
        cpu_ticks[RETIME] += retime/n;
        cpu_ticks[RUTIME] += rutime/n;
        cpu_ticks[STIME] += stime/n;
        break;

      case 1:
        printf(1, "Process pid: %i, type: S-CPU-Bound, retime: %i, rutime: %i, stime: %i", pid, retime, rutime, stime);
        s_ticks[RETIME] += retime/n;
        s_ticks[RUTIME] += rutime/n;
        s_ticks[STIME] += stime/n;
        break;

      case 2:
        printf(1, "Process pid: %i, type: IO-Bound, retime: %i, rutime: %i, stime: %i", pid, retime, rutime, stime);
        io_ticks[RETIME] += retime/n;
        io_ticks[RUTIME] += rutime/n;
        io_ticks[STIME] += stime/n;
        break;
    }
  }

  printf(1, "\nCPU-Bound:\nTempo medio no estado de SLEEP: %d\nTempo medio no estado READY: %d\nTempo medio de turnaround: %d\n", cpu_ticks[STIME], cpu_ticks[RETIME], (cpu_ticks[RUTIME] + cpu_ticks[RETIME] + cpu_ticks[STIME]));

  printf(1, "\nS-CPU-Bound:\nTempo medio no estado de SLEEP: %d\nTempo medio no estado READY: %d\nTempo medio de turnaround: %d\n", s_ticks[STIME], s_ticks[RETIME], (s_ticks[RUTIME] + s_ticks[RETIME] + s_ticks[STIME]));

  printf(1, "\nIO-Bound:\nTempo medio no estado de SLEEP: %d\nTempo medio no estado READY: %d\nTempo medio de turnaround: %d\n", io_ticks[STIME], io_ticks[RETIME], (io_ticks[RUTIME] + io_ticks[RETIME] + io_ticks[STIME]));
  return 0;
}
