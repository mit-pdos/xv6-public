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

/**
 * Initialize the cpu account structure.
 * Call this function before any other ones.
 */
void cpu_account_initialize(struct cpu_account * cpu);

/**
 * Event callback function to signal the cpu account mechanism
 * that the scheduling process has started.
 */
void cpu_account_schedule_start(struct cpu_account * cpu);

/**
 * Event callback function to let the cpu account update the proc structure
 * according to the cpu account mechanism.
 */
void cpu_account_schedule_proc_update(struct cpu_account * cpu, struct proc * p);

/**
 * Make a decision whether the given process can be
 * scheduled.
 * Returns zero for cannot schedule or nonzero in case can schecule.
 */
int cpu_account_schedule_process_decision(struct cpu_account * cpu, struct proc * p);

/**
 * Event callback function to let the cpu account mechanism know that a process is
 * about to be scheduled.
 */
void cpu_account_before_process_schedule(struct cpu_account * cpu, struct proc * p);

/**
 * Event callback function to let the cpu account mechanism know that a process that
 * was scheduled has finished running.
 */
void cpu_account_after_process_schedule(struct cpu_account * cpu, struct proc * p);

/**
 * Event callback function to let the cpu account mechanism know
 * that the scheduling has finished a cycle.
 */
void cpu_account_schedule_finish(struct cpu_account * cpu);

/**
 * Event callback function to let the cpu account mechanism know
 * that the scheduling is about to halt into idle state.
 */
void cpu_account_before_hlt(struct cpu_account * cpu);

/**
 * Event callback function to let the cpu account mechanism know
 * that the scheduling has returned from idle state.
 */
void cpu_account_after_hlt(struct cpu_account * cpu);

#endif
