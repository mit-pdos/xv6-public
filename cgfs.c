#include "cgfs.h"
#include "defs.h"
#include "fcntl.h"
#include "file.h"
#include "fs.h"
#include "param.h"
#include "spinlock.h"
#include "types.h"

#define MAX_PID_LENGTH 5
#define MAX_CGROUP_DIR_ENTRIES 64

#define CGROUP_PROCS 1
#define CGROUP_SUBTREE_CONTROL 2
#define CGROUP_MAX_DESCENDANTS 3
#define CGROUP_MAX_DEPTH 4
#define CGROUP_CONTROLLERS 5
#define CGROUP_EVENTS 6
#define CGROUP_STAT 7

#define CPU_WEIGHT 8
#define CPU_MAX 9
#define CPU_STAT 10


static int fdalloc(struct file * f)
{
    int fd;
    struct proc * curproc = myproc();

    for (fd = 0; fd < NOFILE; fd++) {
        if (curproc->ofile[fd] == 0) {
            curproc->ofile[fd] = f;
            return fd;
        }
    }
    return -1;
}

static int find_procs_offsets(int * procoff, int * pidoff, struct file * f)
{
    *procoff = 0;
    *pidoff = f->off;
    while (*procoff < sizeof(f->cgp->proc) / sizeof(*f->cgp->proc)) {
        if (f->cgp->proc[*procoff] == 0) {
            *procoff += 1;
            continue;
        }
        int pid = proc_pid(f->cgp->proc[*procoff]);
        int pidlen = 1;
        while (pid > 0) {
            pidlen++;
            pid /= 10;
        }
        if (*pidoff >= pidlen) {
            *pidoff -= pidlen;
            *procoff += 1;
        } else
            break;
    }

    if (*procoff >= sizeof(f->cgp->proc) / sizeof(*f->cgp->proc))
        return -1;

    return 0;
}

static int copy_until_char(char * s, char * t, char ch, int n)
{
    int len = 0;
    while (*t != ch && *t != '\0' && (n--) > 0) {
        *s++ = *t++;
        len++;
    }

    *s = 0;
    if (*t == ch)
        len++;

    return len;
}

static int get_file_name_constant(char * filename)
{
    if (strcmp(filename, "cgroup.procs") == 0)
        return CGROUP_PROCS;
    else if(strcmp(filename, "cgroup.subtree_control") == 0)
        return CGROUP_SUBTREE_CONTROL;
    else if(strcmp(filename, "cgroup.max.descendants") == 0)
        return CGROUP_MAX_DESCENDANTS;
    else if(strcmp(filename, "cgroup.max.depth") == 0)
        return CGROUP_MAX_DEPTH;
    else if(strcmp(filename, "cgroup.controllers") == 0)
        return CGROUP_CONTROLLERS;
    else if(strcmp(filename, "cgroup.events") == 0)
        return CGROUP_EVENTS;
    else if(strcmp(filename, "cgroup.stat") == 0)
        return CGROUP_STAT;
    else if(strcmp(filename, "cpu.weight") == 0)
        return CPU_WEIGHT;
    else if(strcmp(filename, "cpu.max") == 0)
        return CPU_MAX;
    else if(strcmp(filename, "cpu.stat") == 0)
        return CPU_STAT;

    return -1;
}

int unsafe_cg_open(cg_file_type type, char * filename, struct cgroup * cgp, int omode)
{
    struct file * f;
    int fd;

    if (type == CG_FILE){

        char writable;
        int filename_const = get_file_name_constant(filename);

        /* Check that the file to be opened is one of the filesystem files and
         * set writeable accordingly.*/
        switch (filename_const) {
            case CGROUP_PROCS:
            case CGROUP_SUBTREE_CONTROL:
            case CGROUP_MAX_DESCENDANTS:
            case CGROUP_MAX_DEPTH:
            case CPU_WEIGHT:
            case CPU_MAX:
                writable = 1;
                break;

            case CGROUP_CONTROLLERS:
            case CGROUP_EVENTS:
            case CGROUP_STAT:
            case CPU_STAT:
                writable = 0;
                break;

            default:
              return -1;
        }

        /* Allocate file structure and file desctiptor.*/
        if ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
            if (f)
                fileclose(f);
            return -1;
        }


        switch (filename_const) {
            case CPU_STAT:
                if (cgp == cgroup_root())
                    return -1;
                f->cpu.stat.active = cgp->cpu_controller_enabled;
                f->cpu.stat.usage_usec = cgp->cpu_time;
                f->cpu.stat.user_usec = cgp->cpu_time;
                f->cpu.stat.system_usec = 0;
                f->cpu.stat.nr_periods = cgp->cpu_nr_periods;
                f->cpu.stat.nr_throttled = cgp->cpu_nr_throttled;
                f->cpu.stat.throttled_usec = cgp->cpu_throttled_usec;
                break;

            case CPU_WEIGHT:
                if (cgp == cgroup_root())
                    return -1;
                f->cpu.weight.weight = 0;
                break;

            case CPU_MAX:
                if (cgp == cgroup_root())
                    return -1;
                f->cpu.max.max = cgp->cpu_time_limit;
                f->cpu.max.period = cgp->cpu_account_period;
                break;
        }

        f->type = FD_CG;
        f->off = 0;
        f->readable = !(omode & O_WRONLY);
        f->writable = ((omode & O_WRONLY) || (omode & O_RDWR)) && writable;
        f->cgp = cgp;
        strncpy(f->cgfilename, filename, sizeof(f->cgfilename));

        cgp->ref_count++;
        return fd;

    }else if (type == CG_DIR){

        if (*cgp->cgroup_dir_path == 0)
            return -1;

        /* Allocate file structure and file desctiptor.*/
        if ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
            if (f)
                fileclose(f);
            return -1;
        }

        f->type = FD_CG;
        f->off = 0;
        f->readable = !(omode & O_WRONLY);
        f->writable = 0;
        f->cgp = cgp;
        *f->cgfilename = 0;

        cgp->ref_count++;
        return fd;
    }

    return -1;
}

int unsafe_cg_read(cg_file_type type, struct file * f, char * addr, int n)
{
    if (type == CG_FILE){

        int r = 0;
        int filename_const = get_file_name_constant(f->cgfilename);

        if (f->readable == 0 || *f->cgp->cgroup_dir_path == 0)
            return -1;

        if (filename_const == CGROUP_PROCS) {
            int procoff;
            int pidoff;
            if (find_procs_offsets(&procoff, &pidoff, f) < 0) {
                return 0;
            }

            while (procoff < (sizeof(f->cgp->proc) / sizeof(*f->cgp->proc)) &&
                   r < n) {
                if (f->cgp->proc[procoff] == 0) {
                    procoff++;
                    continue;
                }

                char buf[MAX_PID_LENGTH];
                int pidlength = itoa(buf, proc_pid(f->cgp->proc[procoff]));
                if (pidoff < pidlength) {
                    *addr = buf[pidoff];
                    pidoff++;
                } else {
                    *addr = '\n';
                    pidoff = 0;
                    procoff++;
                }
                addr++;
                r++;
            }
        } else if (filename_const == CGROUP_CONTROLLERS) {
            if (f->cgp->cpu_controller_avalible) {
                char controllerslist[] = "cpu\n";
                while (r < n && (r + f->off) < sizeof(controllerslist) - 1 &&
                       (*addr++ = controllerslist[(r++) + f->off]) != 0)
                    ;
            }
        } else if (filename_const == CGROUP_SUBTREE_CONTROL) {
            if (f->cgp->cpu_controller_enabled) {
                char enabledcontrollerslist[] = "cpu\n";
                while (r < n &&
                       (r + f->off) < sizeof(enabledcontrollerslist) - 1 &&
                       (*addr++ = enabledcontrollerslist[r++ + f->off]) != 0)
                    ;
            }
        } else if (filename_const == CGROUP_EVENTS) {
            char eventstext[] = "populated - 0\n";
            if (f->cgp->populated)
                eventstext[sizeof(eventstext) - 3] = '1';

            while (r < n && (r + f->off) < sizeof(eventstext) - 1 &&
                   (*addr++ = eventstext[r++ + f->off]) != 0)
                ;
        } else if (filename_const == CGROUP_MAX_DESCENDANTS) {
            char buf[MAX_DECS_SIZE];
            itoa(buf, f->cgp->max_descendants_value);
            while (r < n &&
                   (r + f->off) < sizeof(buf)) {
                if (!buf[r + f->off]) {
                    *addr++ = '\n';
                    ++r;
                    break;
                }

                *addr++ = buf[r + f->off];
                ++r;
            }
        } else if (filename_const == CGROUP_MAX_DEPTH) {
            char buf[MAX_DEPTH_SIZE];
            itoa(buf, f->cgp->max_depth_value);
            while (r < n && (r + f->off) < sizeof(buf)) {
                if (!buf[r + f->off]) {
                    *addr++ = '\n';
                    ++r;
                    break;
                }

                *addr++ = buf[r + f->off];
                ++r;
            }
        } else if (filename_const == CGROUP_STAT) {
            char nr_descendants_buf[MAX_DECS_SIZE];
            itoa(nr_descendants_buf, f->cgp->nr_descendants);
            char nr_dying_descendants_buf[MAX_DECS_SIZE];
            itoa(nr_dying_descendants_buf, f->cgp->nr_dying_descendants);
            char stattext[strlen("nr_descendants - ") +
                          strlen(nr_descendants_buf) + strlen("\n") +
                          strlen("nr_dying_descendants - ") +
                          strlen(nr_dying_descendants_buf) + 2];
            char * stattextp = stattext;
            strncpy(
                stattextp, "nr_descendants - ", sizeof("nr_descendants - "));
            stattextp += strlen("nr_descendants - ");
            strncpy(stattextp,
                    nr_descendants_buf,
                    sizeof(nr_descendants_buf));
            stattextp += strlen(nr_descendants_buf);
            strncpy(stattextp, "\n", sizeof("\n"));
            stattextp += strlen("\n");
            strncpy(stattextp,
                    "nr_dying_descendants - ",
                    sizeof("nr_dying_descendants - "));
            stattextp += strlen("nr_dying_descendants - ");
            strncpy(stattextp,
                    nr_dying_descendants_buf,
                    sizeof(nr_dying_descendants_buf));
            stattext[sizeof(stattext) - 1] = '\n';

            while (r < n && (r + f->off) < sizeof(stattext) &&
                   (*addr++ = stattext[r++ + f->off]) != 0)
                ;
        } else if (filename_const == CPU_STAT) {
            char buf[11];
            char stattext[strlen("usage_usec - ") +
                          itoa(buf, f->cpu.stat.usage_usec) + strlen("\n") +
                          strlen("user_usec - ") +
                          itoa(buf, f->cpu.stat.user_usec) + strlen("\n") +
                          strlen("system_usec - ") +
                          itoa(buf, f->cpu.stat.system_usec) + strlen("\n") +
                          strlen("nr_periods - ") +
                          itoa(buf, f->cpu.stat.nr_periods) + strlen("\n") +
                          strlen("nr_throttled - ") +
                          itoa(buf, f->cpu.stat.nr_throttled) + strlen("\n") +
                          strlen("throttled_usec - ") +
                          itoa(buf, f->cpu.stat.throttled_usec) + 2];
            char * stattextp = stattext;

            strncpy(stattextp, "usage_usec - ", sizeof("usage_usec - "));
            stattextp += strlen("usage_usec - ");
            itoa(buf, f->cpu.stat.usage_usec);
            strncpy(stattextp, buf, sizeof(buf));
            stattextp += strlen(buf);
            strncpy(stattextp, "\n", sizeof("\n"));
            stattextp += strlen("\n");
            strncpy(stattextp, "user_usec - ", sizeof("user_usec - "));
            stattextp += strlen("user_usec - ");
            itoa(buf, f->cpu.stat.user_usec);
            strncpy(stattextp, buf, sizeof(buf));
            stattextp += strlen(buf);
            strncpy(stattextp, "\n", sizeof("\n"));
            stattextp += strlen("\n");
            strncpy(stattextp, "system_usec - ", sizeof("system_usec - "));
            stattextp += strlen("system_usec - ");
            itoa(buf, f->cpu.stat.system_usec);
            strncpy(stattextp, buf, sizeof(buf));
            stattextp += strlen(buf);

            if (f->cpu.stat.active) {
                strncpy(stattextp, "\n", sizeof("\n"));
                stattextp += strlen("\n");
                strncpy(stattextp, "nr_periods - ", sizeof("nr_periods - "));
                stattextp += strlen("nr_periods - ");
                itoa(buf, f->cpu.stat.nr_periods);
                strncpy(stattextp, buf, sizeof(buf));
                stattextp += strlen(buf);
                strncpy(stattextp, "\n", sizeof("\n"));
                stattextp += strlen("\n");
                strncpy(
                    stattextp, "nr_throttled - ", sizeof("nr_throttled - "));
                stattextp += strlen("nr_throttled - ");
                itoa(buf, f->cpu.stat.nr_throttled);
                strncpy(stattextp, buf, sizeof(buf));
                stattextp += strlen(buf);
                strncpy(stattextp, "\n", sizeof("\n"));
                stattextp += strlen("\n");
                strncpy(stattextp,
                        "throttled_usec - ",
                        sizeof("throttled_usec - "));
                stattextp += strlen("throttled_usec - ");
                itoa(buf, f->cpu.stat.throttled_usec);
                strncpy(stattextp, buf, sizeof(buf));
                stattextp += strlen(buf);
            }

            strncpy(stattextp, "\n", sizeof("\n"));

            int stattextlen = strlen(stattext);
            while (r < n && (r + f->off) < stattextlen &&
                   (*addr++ = stattext[r++ + f->off]) != 0)
                ;
        } else if (filename_const == CPU_WEIGHT) {
            char buf[11];
            char weighttext[strlen("weight - ") +
                            itoa(buf, f->cpu.weight.weight) + 2];
            char * weighttextp = weighttext;

            strncpy(weighttextp, "weight - ", sizeof("weight - "));
            weighttextp += strlen("weight - ");
            itoa(buf, f->cpu.weight.weight);
            strncpy(weighttextp, buf, sizeof(buf));

            weighttextp += strlen(buf);
            strncpy(weighttextp, "\n", sizeof("\n"));

            int weighttextlen = strlen(weighttext);
            while (r < n && (r + f->off) < weighttextlen &&
                   (*addr++ = weighttext[r++ + f->off]) != 0)
                ;
        } else if (filename_const == CPU_MAX) {
            char buf[11];
            char maxtext[strlen("max - ") + itoa(buf, f->cpu.max.max) +
                         strlen("\n") + strlen("period - ") +
                         itoa(buf, f->cpu.max.period) + 2];
            char * maxtextp = maxtext;

            strncpy(maxtextp, "max - ", sizeof("max - "));
            maxtextp += strlen("max - ");
            utoa(buf, f->cpu.max.max);
            strncpy(maxtextp, buf, sizeof(buf));
            maxtextp += strlen(buf);
            strncpy(maxtextp, "\n", sizeof("\n"));
            maxtextp += strlen("\n");
            strncpy(maxtextp, "period - ", sizeof("period - "));
            maxtextp += strlen("period - ");
            itoa(buf, f->cpu.max.period);
            strncpy(maxtextp, buf, sizeof(buf));

            maxtextp += strlen(buf);
            strncpy(maxtextp, "\n", sizeof("\n"));

            int maxttextlen = strlen(maxtext);
            while (r < n && (r + f->off) < maxttextlen &&
                   (*addr++ = maxtext[r++ + f->off]) != 0)
                ;
        }

        f->off += r;

        return r;

    }else if (type == CG_DIR){

        char buf[MAX_CGROUP_FILE_NAME_LENGTH * MAX_CGROUP_DIR_ENTRIES];

        if (*f->cgp->cgroup_dir_path == 0)
            return -1;

        for (int i = 0; i < sizeof(buf); i++)
            buf[i] = ' ';

        char * bufp = buf;

        strncpy(bufp, ".", strlen("."));
        bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        if (f->cgp != cgroup_root()) {
            strncpy(bufp, "..", strlen(".."));
            bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        }
        strncpy(bufp, "cgroup.procs", strlen("cgroup.procs"));
        bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        if (f->cgp != cgroup_root()) {
            strncpy(bufp, "cgroup.controllers", strlen("cgroup.controllers"));
            bufp += MAX_CGROUP_FILE_NAME_LENGTH;
            strncpy(bufp,
                    "cgroup.subtree_control",
                    strlen("cgroup.subtree_control"));
            bufp += MAX_CGROUP_FILE_NAME_LENGTH;
            strncpy(bufp, "cgroup.events", strlen("cgroup.events"));
            bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        }
        strncpy(
            bufp, "cgroup.max.descendants", strlen("cgroup.max.descendants"));
        bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        strncpy(bufp, "cgroup.max.depth", strlen("cgroup.max.depth"));
        bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        strncpy(bufp, "cgroup.stat", strlen("cgroup.stat"));
        bufp += MAX_CGROUP_FILE_NAME_LENGTH;
        if (f->cgp != cgroup_root()) {
            strncpy(bufp, "cpu.stat", strlen("cpu.stat"));
            bufp += MAX_CGROUP_FILE_NAME_LENGTH;
            if (f->cgp->cpu_controller_enabled) {
                strncpy(bufp, "cpu.weight", strlen("cpu.weight"));
                bufp += MAX_CGROUP_FILE_NAME_LENGTH;
                strncpy(bufp, "cpu.max", strlen("cpu.max"));
                bufp += MAX_CGROUP_FILE_NAME_LENGTH;
            }
        }

        get_cgroup_names_at_path(bufp, f->cgp->cgroup_dir_path);

        int i;
        for (i = f->off; i < sizeof(buf) && i - f->off < n; i++) {
            *addr++ = buf[i];
        }

        i = i - f->off;
        f->off += i;
        return i;
    }

    return -1;
}

int unsafe_cg_write(struct file * f, char * addr, int n)
{
    int r = 0;
    int filename_const = get_file_name_constant(f->cgfilename);

    if (f->writable == 0 || *f->cgp->cgroup_dir_path == 0)
        return -1;

    if (filename_const == CGROUP_PROCS) {
        char buf[n + 1];
        buf[n] = 0;
        strncpy(buf, addr, n);
        int pid = atoi(buf);
        if (pid <= 0 || cgroup_move_proc(f->cgp, pid) == -1)
            return -1;
        r = n;
    } else if (filename_const == CGROUP_SUBTREE_CONTROL) {
        char cpucontroller = 0; // change to 1 if need to enable, 2 if need
                                // to disable, 0 if nothing to change
        char ch =' ';
        while (*addr != '\0' && n > 0) {
            if (*addr != '+' && *addr != '-')
                return -1;

            char buf[MAX_CONTROLLER_NAME_LENGTH];
            int len = copy_until_char(buf, addr + 1,ch, n - 1);
            if (strcmp(buf, "cpu") == 0) {
                if (*addr == '+')
                    cpucontroller = 1;
                if (*addr == '-')
                    cpucontroller = 2;
                addr += len + 1;
                n -= len + 1;
            } else
                return -1;
        }

        if (cpucontroller == 1 && unsafe_enable_cpu_controller(f->cgp) < 0)
            return -1;
        if (cpucontroller == 2 &&
            unsafe_disable_cpu_controller(f->cgp) < 0)
            return -1;

        r = n;
    } else if (filename_const == CGROUP_MAX_DESCENDANTS) {
        if (atoi(addr) < 0 || strlen(addr) > 2)
            return -1;
        f->cgp->max_descendants_value = atoi(addr);
        r = n;
    } else if (filename_const == CGROUP_MAX_DEPTH) {
        if (atoi(addr) < 0 || strlen(addr) > 2)
            return -1;
        f->cgp->max_depth_value = atoi(addr);
        r = n;
    } else if (filename_const == CPU_MAX &&
               f->cgp->cpu_controller_enabled) {
        char max_string[32] = {0};
        char period_string[32] = {0};
        int max = -1;
        int period = -1;
        int i = 0;

        //sh.c doesn't treat space inside for example: "1000 20000" as a single argument
        //so we will use special format, this also allows to parse zeroes inside a value

        while (*addr && *addr != ',' &&  *addr != '\0' && i < sizeof(max_string)) {
            max_string[i] = *addr;
            i++;
            addr++;
        }
        max_string[i] = '\0';
        i=0;
        addr++;

        while (*addr && *addr != '\0' && i < sizeof(period_string)) {
            period_string[i] = *addr;
            i++;
            addr++;
        }
        period_string[i] = '\0';

        // Update max.
        max = atoi(max_string);
        if (-1 == max) {
            return -1;
        }

        // Update period.
        if (period_string[0]) {
            period = atoi(period_string);
            if (-1 == period) {
                return -1;
            }

            // Update period fields.
            f->cgp->cpu_account_period = period;
            f->cpu.max.period = period;
        }

        // Update max fields.
        f->cgp->cpu_time_limit = max;
        f->cpu.max.max = max;

        r = n;
    }

    return r;
}

int unsafe_cg_close(struct file * file)
{
    fileclose(file);
    file->cgp->ref_count--;
    if (file->cgp->ref_count == 0 && *file->cgp->cgroup_dir_path == 0)
        decrement_nr_dying_descendants(file->cgp->parent);
    return 0;
}

int get_base_name(char * path, char * base_name)
{
    char fpath[MAX_PATH_LENGTH];
    format_path(fpath, path);

    char * file_name_temp = fpath;
    char * temp = fpath;
    while (*temp != '\0') {
        if (*temp == '/')
            file_name_temp = temp;
        temp++;
    }

    if (file_name_temp == fpath || file_name_temp == temp - 1)
        return -1;

    temp = file_name_temp + 1;
    while (*temp != '\0')
        *base_name++ = *temp++;

    *base_name = 0;

    return 0;
}

int get_dir_name(char * path, char * dir_name)
{
    char fpath[MAX_PATH_LENGTH];
    format_path(fpath, path);

    char * file_name_temp = fpath;
    char * temp = fpath;
    while (*temp != '\0') {
        if (*temp == '/')
            file_name_temp = temp;
        temp++;
    }

    if (file_name_temp == fpath || file_name_temp == temp - 1)
        return -1;

    temp = fpath;
    while (temp < file_name_temp)
        *dir_name++ = *temp++;

    *dir_name = 0;

    return 0;
}

static int cg_file_size(struct file * f)
{
    int size = 0;
    int filename_const = get_file_name_constant(f->cgfilename);

    if (filename_const == CGROUP_PROCS) {
        int procoff = 0;
        while (procoff < (sizeof(f->cgp->proc) / sizeof(*f->cgp->proc))) {
            if (f->cgp->proc[procoff] == 0) {
                procoff++;
                continue;
            }
            int i = proc_pid(f->cgp->proc[procoff]);
            while (i != 0) {
                i /= 10;
                size++;
            }
            size++;
            procoff++;
        }
    } else if (filename_const == CGROUP_CONTROLLERS) {
        if (f->cgp->cpu_controller_avalible)
            size += 3;
    } else if (filename_const == CGROUP_SUBTREE_CONTROL) {
        if (f->cgp->cpu_controller_enabled)
            size += 3;
    } else if (filename_const == CGROUP_EVENTS) {
        size += strlen("populated - 0");
    } else if (filename_const == CGROUP_MAX_DESCENDANTS) {
        size += intlen(f->cgp->max_descendants_value);
    } else if (filename_const == CGROUP_MAX_DEPTH) {
        size += intlen(f->cgp->max_depth_value);
    } else if (filename_const == CGROUP_STAT) {
        size += strlen("nr_descendants - ") +
                intlen(f->cgp->nr_descendants) + strlen("\n") +
                strlen("nr_dying_descendants - ") +
                intlen(f->cgp->nr_dying_descendants);
    }

    return size;
}

int unsafe_cg_stat(struct file * f, struct stat * st)
{
    if (*f->cgp->cgroup_dir_path == 0)
        return -1;
    if (*f->cgfilename == 0) {
        st->type = T_CGDIR;
        st->size = MAX_CGROUP_FILE_NAME_LENGTH *
                   (7 + cgorup_num_of_immidiate_children(f->cgp));
    } else {
        st->type = T_CGFILE;
        st->size = cg_file_size(f);
    }
    return 0;
}
