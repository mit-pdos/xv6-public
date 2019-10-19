#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int n;
  int pid, child_pid;
  int prio;
  int *ordem_entrada;
  int *ordem_saida;

  if(argc != 2) {
    printf(1, "Numero de argumentos errados!\n");
    return -1;
  }

  n = atoi(argv[1]);
  ordem_saida = malloc(n*__SIZEOF_INT__);
  ordem_entrada = malloc(n*__SIZEOF_INT__);
  
  for(int i = 0; i < n; i++) {
    pid = fork();
    if(pid < 0) {
      printf(1,"Fork falhou!\n");  
      break;
    }
    //Child process
    if(pid == 0) {
        child_pid = getpid();
        prio = (child_pid % 3) + 1;
        set_prio(prio);

        //CPU-Bound process    
        for(int k = 0; k < 100; k++) {
            for(int l = 0; l < 1000000; l++) {
                //Do nothing
            }
        }
        //Child process ends here
        exit();
    }else{

      //Seta ordem que os processos começam
      ordem_entrada[i] = pid;

      //Continua a criar processos filhos
      continue;
    }
  }

  for(int i = 0; i < n; i++){
      //Espera os filhos terminarem
      pid = wait();
      ordem_saida[i] = pid;
      printf(1,"Processo %d terminou\n", pid);
  }
  
  printf(1, "Ordem de entrada: ");
  for(int i = 0; i < n; i++){
    printf(1, "%d ", ordem_entrada[i]);
  }
  printf(1, "\n");

  printf(1, "Ordem de saida:   ");
  for(int i = 0; i < n; i++){
    printf(1, "%d ", ordem_saida[i]);
  }
  printf(1, "\n");

  free(ordem_saida);
  free(ordem_entrada);
  exit();
}