#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


/**
 * Usage: ctrl_grp [str] [filename]
 * e.g. ctrl_grp +cpu cgroup.subtree_control
 */

int
main(int argc, char *argv[])
{
    char *str = argv[1];
    char *file = argv[2];

    int cgroup_fd = open(file, O_RDWR);


    if(cgroup_fd < 0){
        printf(stderr, "Failed to open %s\n", file);
        exit(1);
    }

    if(write(cgroup_fd, str, sizeof(str)) < 0){
        printf(stderr, "Failed to write %s to %s\n", str, file);
        exit(1);
    }
    if(close(cgroup_fd) < 0){
        printf(stderr, "Failed to close\n");
        exit(1);
    }

  exit(0);
}
