#ifndef XV6_CPU_ACCOUNT_H
#define XV6_CPU_ACCOUNT_H

#include "cgroup.h"

struct cpu_account
{
    unsigned int now;
    unsigned int cpu_account_period;
    unsigned int cpu_account_frame;
    unsigned int process_cpu_time;
    struct cgroup * cgroup;

};

void cpu_account_initialize(struct cpu_account * cpu);
void cpu_account_schedule_start(struct cpu_account * cpu);
void cpu_account_schedule_proc_update(struct cpu_account * cpu, struct proc * p);
int cpu_account_schedule_process_decision(struct cpu_account * cpu, struct proc * p);
void cpu_account_before_process_schedule(struct cpu_account * cpu, struct proc * p);
void cpu_account_after_process_schedule(struct cpu_account * cpu, struct proc * p);
void cpu_account_schedule_finish(struct cpu_account * cpu);
void cpu_account_before_hlt(struct cpu_account * cpu);
void cpu_account_after_hlt(struct cpu_account * cpu);

#endif