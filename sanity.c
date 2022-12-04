#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void simulateCPUBursts(){
    int i;
    for (i = 0; i < 100; i++){
        int j;
        for (j = 0; j < 1000000; j++){
            asm("nop");
        }
    }
}

void simulateShortCPUBursts(){
    int i;
    for (i = 0; i < 100; i++){
        int j;
        for (j = 0; j < 1000000; j++){
            asm("nop");
            if (j % 10000 == 0)
                user_yield();
        }
    }
}

void simulateIOBoundProcess() {
    int i;
    for (i = 0; i < 100; i++){
        sleep(1);
    }
}

char* get_type(int i){
    if (i == 0)
        return "CPU-Bound";
    else if (i == 1)
        return "S-Bound";
    else
        return "IO-Bound";
}

void usage(){
    printf(1, "Usage: sanity <n>, where n is an integer >= 0.\n");
}

int main(int argc, char* argv[]){
    if (argc > 2){
        usage();
        exit();
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        usage();
        exit();
    }

    int amount_of_processes[3];
    int ready_time[3];
    int sleeping_time[3];
    int turnaround_time[3];

    int i;
    for (i = 0; i < 3; i++){
        amount_of_processes[i] = 0;
        ready_time[i] = 0;
        sleeping_time[i] = 0;
        turnaround_time[i] = 0;
    }

    // Create children
    for (i = 0; i < 5 * n; i++){
        int pid = fork();

        if (pid==0){
            int pidModThree = getpid() % 3;
            
            if (pidModThree == 0)
                simulateCPUBursts();
            else if (pidModThree == 1)
                simulateShortCPUBursts();
            else
                simulateIOBoundProcess();
            exit();
        } else {
            amount_of_processes[pid % 3]++;
            continue;
        }
    }

    // Wait for children
    for (i = 0; i < 5 * n; i++){
        int retime, rutime, stime;
        int pid_child = wait2(&retime, &rutime, &stime);
        int pid_child_mod_three = pid_child % 3;
        char *type=get_type(pid_child_mod_three);
        printf(1, "Pid %d died. Type=%s, RETIME=%d, RUTIME=%d, STIME=%d\n", pid_child, type, retime, rutime, stime);

        ready_time[pid_child_mod_three] += retime;
        sleeping_time[pid_child_mod_three] += stime;
        turnaround_time[pid_child_mod_three] += retime + rutime + stime;
    }

    // Print stats
    int avg_ready[3];
    int avg_sleeping[3];
    int avg_turnaround[3];
    int j;
    for (j = 0; j < 3; j++) {
        printf(1, "Type %d\n", j);
        printf(1, "Amount of processes: %d\n", amount_of_processes[j]);

        avg_ready[j] = ready_time[j] / amount_of_processes[j];
        avg_sleeping[j] = sleeping_time[j] / amount_of_processes[j];
        avg_turnaround[j] = turnaround_time[j] / amount_of_processes[j];
        
        printf(1, "Average ready time: %d\n", avg_ready[j]);
        printf(1, "Average sleeping time: %d\n", avg_sleeping[j]);
        printf(1, "Average turnaround time: %d\n", avg_turnaround[j]);
        
        printf(1, "\n\n");
    }

    exit();
}
