#include "types.h"
#include "user.h"

enum ticks_types {RETIME, RUTIME, STIME};

int main(int argc, char *argv[]) {
  int n;
  int pid, child_pid;
  int retime = 0, rutime = 0, stime = 0;
  int cpu_ticks[3] = {0}, 
         s_ticks[3] = {0}, 
         io_ticks[3] = {0};

  if(argc != 2) {
    printf(1, "Numero de argumentos errados!\n");
    exit ();
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
          for(int k = 0; k < 100; k+= 1) {
            for(double l = 0.0; l < 100000; l+= 1) {
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
            proc_yield();
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
      //printf(1,"Processo %d terminou\n",child_pid);
      exit();
    }

    //Parent process
    else {
      //Keeps creating children
      continue;
    }
  }

  for(int i = 0; i < 3*n; i++) {
  
    // TAREFA 5: resultados dos testes
    //Wait for a child to end
    pid = wait2(&retime, &rutime, &stime);
    
    switch(pid % 3) {
      case 0:
        //printf(1, "Process pid: %d, type: CPU-Bound, retime: %d, rutime: %d, stime: %d\n", pid, retime, rutime, stime);
        cpu_ticks[RETIME] += retime;
        cpu_ticks[RUTIME] += rutime;
        cpu_ticks[STIME] += stime;
        break;

      case 1:
        //printf(1, "Process pid: %d, type: S-CPU-Bound, retime: %d, rutime: %d, stime: %d\n", pid, retime, rutime, stime);
        s_ticks[RETIME] += retime;
        s_ticks[RUTIME] += rutime;
        s_ticks[STIME] += stime;
        break;

      case 2:
        //printf(1, "Process pid: %d, type: IO-Bound, retime: %d, rutime: %d, stime: %d\n", pid, retime, rutime, stime);
        io_ticks[RETIME] += retime;
        io_ticks[RUTIME] += rutime;
        io_ticks[STIME] += stime;
        break;
    }
  }

  //Calula as médias de ticks
  cpu_ticks[RETIME] /= n;
  cpu_ticks[RUTIME] /= n;
  cpu_ticks[STIME]  /= n;

  s_ticks[RETIME] /= n;
  s_ticks[RUTIME] /= n;
  s_ticks[STIME]  /= n;

  io_ticks[RETIME] /= n;
  io_ticks[RUTIME] /= n;
  io_ticks[STIME]  /= n;

  printf(1, "\nCPU-Bound:\nTempo medio no estado de SLEEP: %d\nTempo medio no estado READY: %d\nTempo medio de turnaround: %d\n", cpu_ticks[STIME], cpu_ticks[RETIME], (cpu_ticks[RUTIME] + cpu_ticks[RETIME] + cpu_ticks[STIME]));

  printf(1, "\nS-CPU-Bound:\nTempo medio no estado de SLEEP: %d\nTempo medio no estado READY: %d\nTempo medio de turnaround: %d\n", s_ticks[STIME], s_ticks[RETIME], (s_ticks[RUTIME] + s_ticks[RETIME] + s_ticks[STIME]));

  printf(1, "\nIO-Bound:\nTempo medio no estado de SLEEP: %d\nTempo medio no estado READY: %d\nTempo medio de turnaround: %d\n", io_ticks[STIME], io_ticks[RETIME], (io_ticks[RUTIME] + io_ticks[RETIME] + io_ticks[STIME]));
  exit();
  return 0;
}
