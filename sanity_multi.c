#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int n;
  int pid, child_pid;
  int prio;

  if(argc != 2) {
    printf(1, "Numero de argumentos errados!\n");
    return -1;
  }

  n = atoi(argv[1]);
  
  for(int i = 0; i < n; i++) {
    pid = fork();
    if(pid < 0) {
        break;
    }
    printf(1, "Criou filho!\n");
    //Child process
    if(pid == 0) {
        child_pid = getpid();
        prio = (child_pid % 3) + 1;
        set_prio(prio);
        printf(1, "Processo %d com prioridade %d comecou!\n", child_pid, prio);

        //CPU-Bound process    
        for(int k = 0; k < 100; k++) {
            for(int l = 0; l < 1000000; l++) {
                //Do nothing
            }
        }
        //Child process ends here
        printf(1, "Processo %d com prioridade %d terminado!\n", child_pid, prio);
        exit();
    }
    printf(1, "Criou prox filho!\n");
    //Keeps creating children
    continue;
  }
  for(int i = 0; i < n; i++){
      wait();
  }
  exit();
}