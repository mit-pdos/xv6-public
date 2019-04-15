#include "types.h"
#include "user.h"
#include "uproc.h"

void sort_table(struct uproc *table, int procnum)
{
  int i, j;
  struct uproc key;

  for (i = 1; i < procnum; i++) {
    key = table[i];
    j = i-1;

    while (j >= 0 && table[i].state > key.state) {
      table[j+1] = table[j];
      j--;
    }
  }

}

int
main(int argc, char* argv[])
{
  int max = 64;		// NPROCS = 64
  int procnum = 0;
  struct uproc *table;

  table = (struct uproc*)malloc(64*sizeof(struct uproc));
  if(table == 0){
    printf(1, "Out of memory\n");
    return -1;
  }

  // Get the table of processes and return number of processes
  procnum = getprocs(max, table);

  // Parse command-line option
  if (argc > 1) {
    
    // Sort by process state
    if (strcmp(argv[1], "-sp") == 0) {
      printf(1, "Found sp option\n");
      //sort_state(table, procnum);
    } 
    else if (strcmp(argv[1], "-n") == 0){
      printf(1, "Number of processes: %d\n", procnum);
    }
    
    // Search by process name
    else if (strcmp(argv[1], "-s") == 0) {
    }
  }


  printf(1, "PID  PPID  STATE TIME UPTIME  SIZE  NAME\n");
  for(int i = 0; i < procnum; i++) // Print information for each process
  {
    printf(1, " %d   ", table[i].pid);

    printf(1, "%d  ", table[i].ppid);
    int state = table[i].state;
    if(state == 1)
      printf(1, "EMBRYO  ");
    if(state == 2)
      printf(1, "SLEEPING  ");
    if(state == 3)
      printf(1, "RUNNABLE  ");
    if(state == 4)
      printf(1, "RUNNING  ");
    if(state == 5)
      printf(1, "ZOMBIE  ");
    printf(1, "%d  ", (uptime() - table[i].ticks)/10);
    printf(1, "%d  ", (uptime() - table[i].uptime)/10);
    printf(1, "%d  ", table[i].sz);
    printf(1, "%s\n", table[i].name);
  }
  free(table);
  exit();
}
