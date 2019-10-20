#include "types.h"
#include "user.h"

int main(int argc, char *argv[]) {
  int pid;
  int prio;
  int n_process = 0;
  int *ordem_entrada;
  int *ordem_saida;

  ordem_saida = malloc(15*__SIZEOF_INT__);
  ordem_entrada = malloc(15*__SIZEOF_INT__);
  prio = 1;

  for (int i = 0; i < 15; i++)
  {
    pid = fork();
    if (pid < 0)
    {
      printf(1, "Fork falhou!\n");
      break;
    }

    //Child process
    if (pid == 0)
    {
     //printf(1, "processo %d iniciou\n", child_pid);
      set_prio(prio);

      //CPU-Bound process
      for (int k = 0; k < 100; k++)
      {
        for (int l = 0; l < 1000000; l++)
        {
          //Do nothing
        }
      }
      //Child process ends here
      exit();
    }
    else
    {
      //A cada 5 processos criados, aumenta a prioridade
      n_process++;
      if(n_process == 5) {
        prio++;
        n_process = 0;
      }

      //Seta ordem que os processos começam
      ordem_entrada[i] = pid;

      //Continua a criar processos filhos
      continue;
    }
  }

  for(int i = 0; i < 15; i++){
      //Espera os filhos terminarem
      pid = wait();
      ordem_saida[i] = pid;
      //printf(1,"Processo %d terminou\n", pid);
  }
  
  printf(1, "Ordem de entrada: ");
  for(int i = 0; i < 15; i++){
    printf(1, "%d ", ordem_entrada[i]);
  }
  printf(1, "\n");

  printf(1, "Ordem de saida:   ");
  for(int i = 0; i < 15; i++){
    printf(1, "%d ", ordem_saida[i]);
  }
  printf(1, "\n");

  free(ordem_saida);
  free(ordem_entrada);
  exit();
}
