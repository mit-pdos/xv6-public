#include "types.h"
#include "user.h"
#include "uproc.h"


int search_proc(char* searched, struct uproc *table, int procnum, struct uproc *result);

void sort_size(struct uproc *table, int procnum)
{
  int i, j;
  struct uproc key;

  for (i = 1; i < procnum; i++) {
    key = table[i];
    j = i-1;
    while ((j >= 0) && (table[j].sz > key.sz)) {
      table[j+1] = table[j];
      j--;
    }
    table[j+1] = key;
  }

}

int 
search_proc(char* searched, struct uproc *table, int procnum, struct uproc *result){
  for(int i=0; i<procnum; i++){
    if(strcmp(searched, table[i].name) == 0){
      result->pid = table[i].pid;
      result->ppid = table[i].ppid;
      result->state = table[i].state;
      result->sz = table[i].sz;
      result->uptime = table[i].uptime;
      result->ticks = table[i].ticks;
      result->sleepticks = table[i].sleepticks;
      strcpy(result->name, table[i].name);
      return 1;
    }
  }
  return 0;
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
    // Print number of processes
    else if (strcmp(argv[1], "-n") == 0){
      printf(1, "Number of processes: %d\n", procnum);
    }
    // Sort by process size
    else if (strcmp(argv[1], "-ss") == 0){
      sort_size(table, procnum);
    } 
    // Search by process name
    else
    if((strcmp(argv[1], "-s") == 0)){
      if((argc > 2)){         
        struct uproc *p = (struct uproc *)malloc(sizeof(struct uproc)); 

        if((search_proc(argv[2], table, procnum, p)) == 1){ //create this function 
          printf(1, "PID  PPID  STATE TIME UPTIME  SIZE  NAME\n");
          printf(1, " %d   ", p->pid);
          printf(1, "%d  ", p->ppid);
          int state = p->state;
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
            
          printf(1, "%d  ", (uptime() - p->ticks)/10);
          printf(1, "%d  ", (uptime() - p->uptime)/10);
          printf(1, "%d  ", p->sz);
          printf(1, "%s\n\n", p->name);

        } else {
            printf(1, "process not found\n\n");
        }
      } else {
        printf(1, "usage: no process name given to search\n\n"); //error
      }

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
