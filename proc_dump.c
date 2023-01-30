#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// reverse:  reverse string s in place
void
reverse(char s[])
{
  int c, i, j;

  for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
    c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

// itoa:  convert n to characters in s
void
itoa(char s[], int n)
{
  int i, sign;

  if((sign = n) < 0)  // record sign
    n = -n;          // make n positive
  i = 0;
  do {       // generate digits in reverse order
    s[i++] = n % 10 + '0';   // get next digit
  } while ((n /= 10) > 0);     // delete it
  if(sign < 0)
    s[i++] = '-';
  s[i] = '\0';
  reverse(s);
}


int main(int argc, char *argv[])
{
    /// proc_dump 5 10000 25000 25000 50000 68000 (5 processes add to current)
    // proc_dump 5 100 700000 1200000 2000000 3500000
    for (int i = 0; i < argc; i++)
    {
        int pid = fork();
        if (pid == 0)
        {
            malloc(atoi(argv[i]));
        
            while (1)
            {
            }
            // long tmp = 0;
            // for (unsigned int j = 0; j < 0xFFFFFFFF; j++)
            // {
            //     for (unsigned int k = 0; k < 0xFFFFFFFF; k++)
            //     {
            //         tmp += j * k;
            //     }
            // }
            exit();
        }
    }
    int maxProcess = NPROC; // max number of processes = 64
    struct proc_info *processes = malloc(maxProcess * sizeof(struct proc_info));

    int Number = proc_dump(maxProcess, processes);
    for (int i = 0; i < Number; i++)
    {
        // printf(1, "memsize:%d -- process_id:%d\n", processes[i].memsize, processes[i].pid);
        char str1[50];
        char str2[50];
        itoa(str1, processes[i].memsize);
        itoa(str2, processes[i].pid);
        printf(1, "memsize:");
        printf(1, str1);
        printf(1, " -- process_id:");
        printf(1, str2);
        printf(1, "\n");
    }
    // for (int i = 0; i < argc; i++)
    // {
    //     wait();
    // }

    // for (int i = 0; i < argc; i++)
    // {
    //     kill(pids[i]);
    // }
    exit();

    // -----------------------------------------------

    // int r = fork();
    // if (r == 0)
    // {
    //     r = fork();
    //     if (r != 0)
    //     {
    //         // int: 4 bytes
    //         // 32 bytes of memory block
    //         // malloc(8 * sizeof(int));
    //         malloc(100000);
    //         for (unsigned int i = 0; i < 0xFFFFFFFF; i++)
    //         {
    //         }
    //         for (unsigned int j = 0; j < 0xFFFFFFFF; j++)
    //         {
    //             for (unsigned int k = 0; k < 0xFFFFFFFF; k++)
    //             {
    //             }
    //         }
    //     }
    //     else
    //     {
    //         // 40 bytes of memory block
    //         // malloc(10 * sizeof(int));
    //         // malloc(10000);
    //         malloc(100000);
    //         for (unsigned int i = 0; i < 0xFFFFFFFF; i++)
    //         {
    //         }
    //         for (unsigned int j = 0; j < 0xFFFFFFFF; j++)
    //         {
    //             for (unsigned int k = 0; k < 0xFFFFFFFF; k++)
    //             {
    //             }
    //         }
    //     }
    //     wait();
    // }
    // else
    // {
    //     r = fork();
    //     if (r == 0)
    //     {
    //         // 60 bytes of memory block
    //         // malloc(15 * sizeof(int));
    //         malloc(30000);
    //         for (unsigned int i = 0; i < 0xFFFFFFFF; i++)
    //         {
    //         }
    //         for (unsigned int j = 0; j < 0xFFFFFFFF; j++)
    //         {
    //             for (unsigned int k = 0; k < 0xFFFFFFFF; k++)
    //             {
    //             }
    //         }
    //         // wait();
    //     }
    //     else
    //     {
    //         // 20 bytes of memory block
    //         // malloc(5 * sizeof(int));
    //         malloc(40000);
    //         int maxProcess = NPROC; // max number of processes = 64
    //         struct proc_info *processes = malloc(maxProcess * sizeof(struct proc_info));

    //         int Number = proc_dump(maxProcess, processes);
    //         for (int i = 0; i < Number; i++)
    //         {
    //             // printf(2, processes[i].pid, processes[i].memsize);
    //             printf(1, "memsize:%d -- process_id:%d\n", processes[i].memsize, processes[i].pid);
    //         }
    //         wait();
    //     }
    // }

    // wait();
    // wait();
    // exit();
    // return 0;

    }
