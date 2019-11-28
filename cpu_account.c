#include "cpu_account.h"
#include "steady_clock.h"

void cpu_account_initialize(struct cpu_account * cpu)
{
    cpu->cgroup = 0;
    cpu->cpu_account_frame = 0;
    cpu->cpu_account_period = 1 * 100 * 1000; // 100ms
    cpu->now = 0;
    cpu->process_cpu_time = 0;
}

void cpu_account_schedule_start(struct cpu_account * cpu)
{
    cpu->now = steady_clock_now();
}

void cpu_account_schedule_proc_update(struct cpu_account * cpu, struct proc * p)
{
    // If cpu accounting frame has passed, update CPU accounting.
    if (cpu->cpu_account_frame > p->cpu_account_frame) {
        unsigned int current_cpu_time =
            p->cpu_period_time > cpu->cpu_account_period
                ? cpu->cpu_account_period
                : p->cpu_period_time;
        p->cpu_percent = current_cpu_time * 100 / cpu->cpu_account_period;
        p->cpu_account_frame = cpu->cpu_account_frame;
        p->cpu_period_time -= current_cpu_time;
    }
}

int cpu_account_schedule_process_decision(struct cpu_account * cpu,
                                          struct proc * p)
{
    // The cpu account frame according to the cgroup account period.
    unsigned int cgroup_cpu_account_frame;

    // Whether to schedule or not.
    char schedule = 1;

    // Set the current cgroup.
    struct cgroup * cgroup = p->cgroup;
    cpu->cgroup = cgroup;

    // Lock cgroup table.
    cgroup_lock();

    while (cgroup) {
        // The cgroup cpu account frame.
        cgroup_cpu_account_frame =
            cpu->now / cgroup->cpu_account_period;

        // If cgroup cpu accounting frame is over, start a new one.
        if (cgroup_cpu_account_frame > cgroup->cpu_account_frame) {
            unsigned int current_cpu_time =
                cgroup->cpu_period_time > cgroup->cpu_account_period
                    ? cgroup->cpu_account_period
                    : cgroup->cpu_period_time;
            if (cgroup->cpu_controller_enabled) {
                ++cgroup->cpu_nr_periods;
            }
            cgroup->cpu_is_throttled_period = 0;
            cgroup->cpu_percent =
                current_cpu_time * 100 / cgroup->cpu_account_period;
            cgroup->cpu_account_frame = cgroup_cpu_account_frame;
            cgroup->cpu_period_time -= current_cpu_time;
        }

        // If cpu time is larger than cpu time limit, skip this process.
        if (cgroup->cpu_controller_enabled &&
            cgroup->cpu_period_time > cgroup->cpu_time_limit) {
            // Increase throttled number if not yet done.
            if (!cgroup->cpu_is_throttled_period) {
                ++cgroup->cpu_nr_throttled;
                cgroup->cpu_throttled_usec +=
                    cgroup->cpu_account_period - cgroup->cpu_period_time;
                cgroup->cpu_is_throttled_period = 1;
            }

            // Do not schedule.
            schedule = 0;

            // Advance to parent and continue.
            cgroup = cgroup->parent;
            continue;
        }

        // Advance to parent and continue.
        cgroup = cgroup->parent;
    }

    // Unlock the cgroup lock and return the result.
    cgroup_unlock();
    return schedule;
}

void cpu_account_before_process_schedule(struct cpu_account * cpu,
                                         struct proc * proc)
{
    // Update process cpu time.
    cpu->process_cpu_time = steady_clock_now();
}

void cpu_account_after_process_schedule(struct cpu_account * cpu,
                                        struct proc * p)
{
    struct cgroup * cgroup = cpu->cgroup;

    // Update now.
    cpu->now = steady_clock_now();

    // Update process cpu time.
    cpu->process_cpu_time = cpu->now - cpu->process_cpu_time;

    // Update process cpu time.
    p->cpu_time += cpu->process_cpu_time;
    p->cpu_period_time += cpu->process_cpu_time;

    // Lock the cgroup lock.
    cgroup_lock();

    // Update cgroup cpu time.
    while (cgroup) {
        // Update cgroup cpu time.
        cgroup->cpu_time += cpu->process_cpu_time;
        cgroup->cpu_period_time += cpu->process_cpu_time;

        // If cgroup cpu controller is not enabled.
        if (!cgroup->cpu_controller_enabled) {
            // Advance to parent and continue.
            cgroup = cgroup->parent;
            continue;
        }

        // Advance to parent.
        cgroup = cgroup->parent;
    }

    // Unlock the cgroup lock.
    cgroup_unlock();
}

void cpu_account_schedule_finish(struct cpu_account * cpu)
{
}

void cpu_account_before_hlt(struct cpu_account * cpu)
{
}

void cpu_account_after_hlt(struct cpu_account * cpu)
{
}
