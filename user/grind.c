//
// run random system calls in parallel forever.
//

#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/syscall.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"

// from FreeBSD.
int
do_rand(unsigned long *ctx)
{
/*
 * Compute x = (7^5 * x) mod (2^31 - 1)
 * without overflowing 31 bits:
 *      (2^31 - 1) = 127773 * (7^5) + 2836
 * From "Random number generators: good ones are hard to find",
 * Park and Miller, Communications of the ACM, vol. 31, no. 10,
 * October 1988, p. 1195.
 */
    long hi, lo, x;

    /* Transform to [1, 0x7ffffffe] range. */
    x = (*ctx % 0x7ffffffe) + 1;
    hi = x / 127773;
    lo = x % 127773;
    x = 16807 * lo - 2836 * hi;
    if (x < 0)
        x += 0x7fffffff;
    /* Transform to [0, 0x7ffffffd] range. */
    x--;
    *ctx = x;
    return (x);
}

unsigned long rand_next = 1;

int
rand(void)
{
    return (do_rand(&rand_next));
}

void
go(int which_child)
{
  int fd = -1;
  static char buf[999];
  char *break0 = sbrk(0);
  uint64 iters = 0;
  
  while(1){
    iters++;
    if((iters % 500) == 0)
      write(1, which_child?"B":"A", 1);
    int what = rand() % 20;
    if(what == 1){
      close(open("a", O_CREATE|O_RDWR));
    } else if(what == 2){
      close(open("b", O_CREATE|O_RDWR));
    } else if(what == 3){
      unlink("a");
    } else if(what == 4){
      unlink("b");
    } else if(what == 5){
      close(fd);
      fd = open("a", O_CREATE|O_RDWR);
    } else if(what == 6){
      close(fd);
      fd = open("b", O_CREATE|O_RDWR);
    } else if(what == 7){
      write(fd, buf, sizeof(buf));
    } else if(what == 8){
      read(fd, buf, sizeof(buf));
    } else if(what == 9){
      mkdir("a");
      close(open("a/a", O_CREATE|O_RDWR));
      unlink("a/a");
    } else if(what == 10){
      mkdir("b");
      close(open("b/b", O_CREATE|O_RDWR));
      unlink("b/b");
    } else if(what == 11){
      unlink("b");
      link("a", "b");
    } else if(what == 12){
      unlink("a");
      link("b", "a");
    } else if(what == 13){
      int pid = fork();
      if(pid == 0){
        exit(0);
      } else if(pid < 0){
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } else if(what == 14){
      int pid = fork();
      if(pid == 0){
        fork();
        fork();
        exit(0);
      } else if(pid < 0){
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } else if(what == 15){
      sbrk(6011);
    } else if(what == 16){
      if(sbrk(0) > break0)
        sbrk(-(sbrk(0) - break0));
    } else if(what == 17){
      int pid = fork();
      if(pid == 0){
        close(open("a", O_CREATE|O_RDWR));
        exit(0);
      } else if(pid < 0){
        printf("grind: fork failed\n");
        exit(1);
      }
      kill(pid);
      wait(0);
    } else if(what == 18){
      int pid = fork();
      if(pid == 0){
        kill(getpid());
        exit(0);
      } else if(pid < 0){
        printf("grind: fork failed\n");
        exit(1);
      }
      wait(0);
    } else if(what == 19){
      int fds[2];
      if(pipe(fds) < 0){
        printf("grind: pipe failed\n");
        exit(1);
      }
      int pid = fork();
      if(pid == 0){
        fork();
        fork();
        if(write(fds[1], "x", 1) != 1)
          printf("grind: pipe write failed\n");
        char c;
        if(read(fds[0], &c, 1) != 1)
          printf("grind: pipe read failed\n");
        exit(0);
      } else if(pid < 0){
        printf("grind: fork failed\n");
        exit(1);
      }
      close(fds[0]);
      close(fds[1]);
      wait(0);
    }
  }
}

int
main()
{
  unlink("a");
  unlink("b");
  
  int pid1 = fork();
  if(pid1 < 0){
    printf("grind: fork failed\n");
    exit(1);
  }
  if(pid1 == 0){
    rand_next = 31;
    go(0);
    exit(0);
  }

  int pid2 = fork();
  if(pid2 < 0){
    printf("grind: fork failed\n");
    exit(1);
  }
  if(pid2 == 0){
    rand_next = 7177;
    go(1);
    exit(0);
  }

  int st1 = -1;
  wait(&st1);
  if(st1 != 0){
    kill(pid1);
    kill(pid2);
  }
  int st2 = -1;
  wait(&st2);

  exit(0);
}
