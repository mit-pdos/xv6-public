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

static int findprocsoffsets(int * procoff, int * pidoff, struct file * f)
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

static int copyuntilspace(char * s, char * t, int n)
{
    int len = 0;

    while (*t != ' ' && *t != '\0' && (n--) > 0) {
        *s++ = *t++;
        len++;
    }

    *s = 0;
    if (*t == ' ')
        len++;

    return len;
}

int unsafe_opencgfile(char * filename, struct cgroup * cgp, int omode)
{
    struct file * f;
    int fd;
    char writable;

    /* Check that the file to be opened is one of the filesystem files and
     * set writeable accordingly.*/
    if (strcmp(filename, "cgroup.procs") == 0 ||
        strcmp(filename, "cgroup.subtree_control") == 0 ||
        strcmp(filename, "cgroup.max.descendants") == 0 ||
        strcmp(filename, "cgroup.max.depth") == 0 ||
        strcmp(filename, "cpu.weight") == 0 ||
        strcmp(filename, "cpu.max") == 0)
        writable = 1;
    else if (strcmp(filename, "cgroup.controllers") == 0 ||
             strcmp(filename, "cgroup.events") == 0 ||
             strcmp(filename, "cgroup.stat") == 0 ||
             strcmp(filename, "cpu.stat") == 0)
        writable = 0;
    else
        return -1;

    /* Allocate file structure and file desctiptor.*/
    if ((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if (f)
            fileclose(f);
        return -1;
    }

    if (strcmp(filename, "cpu.stat") == 0) {
        if (cgp == cgroup_root()) {
            return -1;
        }
        f->cpu.stat.active = cgp->cpu_controller_enabled;
        f->cpu.stat.usage_usec = cgp->cpu_time;
        f->cpu.stat.user_usec = cgp->cpu_time;
        f->cpu.stat.system_usec = 0;
        f->cpu.stat.nr_periods = cgp->cpu_nr_periods;
        f->cpu.stat.nr_throttled = cgp->cpu_nr_throttled;
        f->cpu.stat.throttled_usec = cgp->cpu_throttled_usec;
    }

    if (strcmp(filename, "cpu.weight") == 0) {
        if (cgp == cgroup_root()) {
            return -1;
        }
        f->cpu.weight.weight = 0;
    }

    if (strcmp(filename, "cpu.max") == 0) {
        if (cgp == cgroup_root()) {
            return -1;
        }
        f->cpu.max.max = cgp->cpu_time_limit;
        f->cpu.max.period = cgp->cpu_account_period;
    }

    f->type = FD_CG;
    f->off = 0;
    f->readable = !(omode & O_WRONLY);
    f->writable = ((omode & O_WRONLY) || (omode & O_RDWR)) && writable;
    f->cgp = cgp;
    strncpy(f->cgfilename, filename, sizeof(f->cgfilename));

    cgp->ref_count++;

    return fd;
}

int unsafe_opencgdirectory(struct cgroup * cgp, int omode)
{
    struct file * f;
    int fd;

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

int unsafe_readcgfile(struct file * f, char * addr, int n)
{
    int r = 0;

    if (f->readable == 0 || *f->cgp->cgroup_dir_path == 0)
        return -1;

    if (strcmp(f->cgfilename, "cgroup.procs") == 0) {
        int procoff;
        int pidoff;
        if (findprocsoffsets(&procoff, &pidoff, f) < 0) {
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
    } else if (strcmp(f->cgfilename, "cgroup.controllers") == 0) {
        if (f->cgp->cpu_controller_avalible) {
            char controllerslist[] = "cpu\n";
            while (r < n && (r + f->off) < sizeof(controllerslist) - 1 &&
                   (*addr++ = controllerslist[(r++) + f->off]) != 0)
                ;
        }
    } else if (strcmp(f->cgfilename, "cgroup.subtree_control") == 0) {
        if (f->cgp->cpu_controller_enabled) {
            char enabledcontrollerslist[] = "cpu\n";
            while (r < n &&
                   (r + f->off) < sizeof(enabledcontrollerslist) - 1 &&
                   (*addr++ = enabledcontrollerslist[r++ + f->off]) != 0)
                ;
        }
    } else if (strcmp(f->cgfilename, "cgroup.events") == 0) {
        char eventstext[] = "populated - 0\n";
        if (f->cgp->populated)
            eventstext[sizeof(eventstext) - 3] = '1';

        while (r < n && (r + f->off) < sizeof(eventstext) - 1 &&
               (*addr++ = eventstext[r++ + f->off]) != 0)
            ;
    } else if (strcmp(f->cgfilename, "cgroup.max.descendants") == 0) {
        while (r < n &&
               (r + f->off) < sizeof(f->cgp->max_descendants_value)) {
            if (!f->cgp->max_descendants_value[r + f->off]) {
                *addr++ = '\n';
                ++r;
                break;
            }

            *addr++ = f->cgp->max_descendants_value[r + f->off];
            ++r;
        }
    } else if (strcmp(f->cgfilename, "cgroup.max.depth") == 0) {
        while (r < n && (r + f->off) < sizeof(f->cgp->max_depth_value)) {
            if (!f->cgp->max_depth_value[r + f->off]) {
                *addr++ = '\n';
                ++r;
                break;
            }

            *addr++ = f->cgp->max_depth_value[r + f->off];
            ++r;
        }
    } else if (strcmp(f->cgfilename, "cgroup.stat") == 0) {
        char stattext[strlen("nr_descendants - ") +
                      strlen(f->cgp->nr_descendants) + strlen("\n") +
                      strlen("nr_dying_descendants - ") +
                      strlen(f->cgp->nr_dying_descendants) + 2];
        char * stattextp = stattext;
        strncpy(
            stattextp, "nr_descendants - ", sizeof("nr_descendants - "));
        stattextp += strlen("nr_descendants - ");
        strncpy(stattextp,
                f->cgp->nr_descendants,
                sizeof(f->cgp->nr_descendants));
        stattextp += strlen(f->cgp->nr_descendants);
        strncpy(stattextp, "\n", sizeof("\n"));
        stattextp += strlen("\n");
        strncpy(stattextp,
                "nr_dying_descendants - ",
                sizeof("nr_dying_descendants - "));
        stattextp += strlen("nr_dying_descendants - ");
        strncpy(stattextp,
                f->cgp->nr_dying_descendants,
                sizeof(f->cgp->nr_dying_descendants));
        stattext[sizeof(stattext) - 1] = '\n';

        while (r < n && (r + f->off) < sizeof(stattext) &&
               (*addr++ = stattext[r++ + f->off]) != 0)
            ;
    } else if (strcmp(f->cgfilename, "cpu.stat") == 0) {
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
    } else if (strcmp(f->cgfilename, "cpu.weight") == 0) {
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
    } else if (strcmp(f->cgfilename, "cpu.max") == 0) {
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
}

int unsafe_readcgdirectory(struct file * f, char * addr, int n)
{
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

int unsafe_writecgfile(struct file * f, char * addr, int n)
{
    int r = 0;

    if (f->writable == 0 || *f->cgp->cgroup_dir_path == 0)
        return -1;

    if (strcmp(f->cgfilename, "cgroup.procs") == 0) {
        char buf[n + 1];
        buf[n] = 0;
        strncpy(buf, addr, n);
        int pid = atoi(buf);
        if (pid <= 0 || cgroup_move_proc(f->cgp, pid) == -1)
            return -1;
        r = n;
    } else if (strcmp(f->cgfilename, "cgroup.subtree_control") == 0) {
        char cpucontroller = 0; // change to 1 if need to enable, 2 if need
                                // to disable, 0 if nothing to change

        while (*addr != '\0' && n > 0) {
            if (*addr != '+' && *addr != '-')
                return -1;

            char buf[MAX_CONTROLLER_NAME_LENGTH];
            int len = copyuntilspace(buf, addr + 1, n - 1);
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
    } else if (strcmp(f->cgfilename, "cgroup.max.descendants") == 0) {
        if (atoi(addr) < 0 || strlen(addr) > 2)
            return -1;
        strncpy(f->cgp->max_descendants_value,
                addr,
                sizeof(f->cgp->max_descendants_value));
        r = n;
    } else if (strcmp(f->cgfilename, "cgroup.max.depth") == 0) {
        if (atoi(addr) < 0 || strlen(addr) > 2)
            return -1;
        strncpy(f->cgp->max_depth_value,
                addr,
                sizeof(f->cgp->max_descendants_value));
        r = n;
    }

    return r;
}

int unsafe_closecgfileordir(struct file * file)
{
    fileclose(file);
    file->cgp->ref_count--;
    if (file->cgp->ref_count == 0 && *file->cgp->cgroup_dir_path == 0)
        decrement_nr_dying_descendants(file->cgp->parent);
    return 0;
}

int get_cg_file_dir_path_and_file_name(char * path,
                                       char * dir_path,
                                       char * file_name)
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

    if (file_name_temp == fpath || file_name_temp == fpath + 1)
        return -1;

    temp = file_name_temp + 1;
    while (*temp != '\0')
        *file_name++ = *temp++;

    temp = fpath;
    while (temp < file_name_temp)
        *dir_path++ = *temp++;

    *file_name = 0;
    *dir_path = 0;

    return 0;
}

static int cgfilesize(struct file * f)
{
    int size = 0;

    if (strcmp(f->cgfilename, "cgroup.procs") == 0) {
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
    } else if (strcmp(f->cgfilename, "cgroup.controllers") == 0) {
        if (f->cgp->cpu_controller_avalible)
            size += 3;
    } else if (strcmp(f->cgfilename, "cgroup.subtree_control") == 0) {
        if (f->cgp->cpu_controller_enabled)
            size += 3;
    } else if (strcmp(f->cgfilename, "cgroup.events") == 0) {
        size += strlen("populated - 0");
    } else if (strcmp(f->cgfilename, "cgroup.max.descendants") == 0) {
        size += strlen(f->cgp->max_descendants_value);
    } else if (strcmp(f->cgfilename, "cgroup.max.depth") == 0) {
        size += strlen(f->cgp->max_depth_value);
    } else if (strcmp(f->cgfilename, "cgroup.stat") == 0) {
        size += strlen("nr_descendants - ") +
                strlen(f->cgp->nr_descendants) + strlen("\n") +
                strlen("nr_dying_descendants - ") +
                strlen(f->cgp->nr_dying_descendants);
    }

    return size;
}

int unsafe_cgstat(struct file * f, struct stat * st)
{
    if (*f->cgp->cgroup_dir_path == 0)
        return -1;
    if (*f->cgfilename == 0) {
        st->type = T_CGDIR;
        st->size = MAX_CGROUP_FILE_NAME_LENGTH *
                   (7 + cgorup_num_of_immidiate_children(f->cgp));
    } else {
        st->type = T_CGFILE;
        st->size = cgfilesize(f);
    }
    return 0;
}
