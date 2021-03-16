#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "ns_types.h"
#include "param.h"


void
createNwrite(char *path, char *str, int strlen){
         int fd;

         if ((fd = open(path,O_CREATE|O_RDWR)) <= 0){
            printf(stderr, "open failed\n");
            exit(1);
         }

         if (write(fd, str, strlen) != 10){
            printf(stderr, "write failed\n");
            exit(1);
         }

         close (fd);
}

int
main(int argc, char *argv[])
{

     // ******************************************************************************
     // create a child process with a separate mount namespace
     // create a mount point and mount on it a preformatted internal_fs_a
     // create a file on the mounted file system
     int pid = fork();
     if (pid < 0){
        printf(stderr, "fork failed\n");
        exit(1);
     }

     if (pid == 0){
         if(unshare(MOUNT_NS) != 0){
            printf(stderr, "Cannot create mount namespace\n");
            exit(1);
         }
         if (mkdir("dirA") != 0){
            printf(stderr, "mkdir failed\n");
            exit(1);
         }
         if (mount("internal_fs_a","dirA",0) != 0){
            printf(stderr, "mount failed\n");
            exit(1);
         }
         createNwrite("dirA/file.txt", "123456789\n",10);
     }

     // ******************************************************************************
     // ******************************************************************************
     if (pid > 0){
         // make sure child process runs first to create a new ns
         sleep(100);
         // create a mount point and mount on it a preformatted internal_fs_b
         if (mkdir("dirB") != 0){
            printf(stderr, "mkdir failed\n");
            exit(1);
         }
         if (mount("internal_fs_b","dirB",0) != 0){
            printf(stderr, "mount failed\n");
            exit(1);
          }
          createNwrite("dirB/file.txt", "987654321\n",10);
      }

     // ******************************************************************************
     // both processes will sleep for a while to enable each
     // other to reach this point
     sleep(100);
     // ******************************************************************************
     // at this point it is guaranteed that the child process is able to access
     // dirA/file.txt while the parent process is able to access dirB/file.txt but
     // not vice versa. We just need to check it.
     if(pid == 0){
         if (open("dirA/file.txt",O_RDONLY) < 0){
            printf(stderr, "open was about to succeed but failed\n");
            exit(1);
          }
         if (open("dirB/file.txt",O_RDONLY) >= 0){
            printf(stderr, "open was about to fail but succeeded\n");
            exit(1);
          }
      }
      else{
         if (open("dirB/file.txt",O_RDONLY) < 0){
            printf(stderr, "open was about to succeed but failed\n");
            exit(1);
          }
         if (open("dirA/file.txt",O_RDONLY) >= 0){
            printf(stderr, "open was about to fail but succeeded\n");
            exit(1);
          }
      }
      exit(0);
}
