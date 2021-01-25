# Table of content
- [Scheduler](#Scheduler)
    - [Required](#An-xv6-lottery-scheduler)
    - [Implementation](#Steps)
    - [Test](#Test)
- [Virtual memory](#Virtual-memory)
    - [Required](#Required-1)
    - [Implementation](#Implementation-1)
    - [Test](#Test-1)
- [Thread](#Threads)
    - [Required](#Required-2)
    - [Implementation](#Implementation-2)
    - [Test](#Test-2)
- [References](#References)
- [Team](#Team)

---
## Scheduler

## An xv6 lottery scheduler
the xv6 by default use a round robin scheduling algorithm. so we will change it to a lottery scheduler. </br>
The lottery in a nutshell is a probabilistic scheduling algorithm, and the basic idea is to give the processes lottery tickets for various system resources, such as cup time. Whenever a scheduling decision has to be made a lottery ticket is chosen at random, and the process holding that ticket gets the resource. More important processes can be given extra tickets to increase their odds of winning. Lottery scheduling also solves the problem of starvation. Giving each process at least one lottery ticket guarantees that it has a non-zero probability of being selected at each scheduling operation.</br>
In conclusion what the scheduler will be doing is given two processes A and B, if A has 60 tickets from 1-60 and B has 40 tickets from 61-100. The scheduler will pick a random number from 1 to 100 if the picked number is from 1 to 60 then A is the winner else if the picked number is from 61 to 100 then b is the winner.</br>

### we will implement two system calls:
1 - `int settickets(int number)` </br>
Which sets the number of tickets to the calling process. As mentioned above every process has 1 ticket by default, but it may call this routine to raise the number of tickets it has and receive a higher proportion of the cpu.  
2 - `int getpinfo(struct pstat *)`</br>
This system call returns some info about all the running processes such as the process id, number of tickets, how many ticks this process accumulated up til now and whether this process is in use or not. We will make use of this system call in the testing phase of the lottery scheduler, But in general we can use it for any task related to processes information.
it's a variant of the command line program `ps` which is used to know what is going on.

### Steps:
1- Make a system call which allows you to set the tickets for a process.</br>
2- Code to generate a random number.</br>
3- In the scheduler function count the total number of tickets for all processes that are runnable.</br>
4- Generate a random number between 0 and the total tickets calculated above.</br>
5- When looping through the processes keep the counter of the total number of tickets passed.</br>
6- Just when the counter becomes greater the random value we got, run the process.</br>
7- Put a break at the end of for loop so that we don't execute the processes following the process we just run.</br>


#### Making the settickets system call:</br>
First we need to set the process structure up for the system call. So we need to make couple of changes in `proc.c` and `proc.h`. </br>
Go to `proc.h` and in the struct proc add two variables, one for the tickets and the other for the ticks. </br>
```
int tickets;
int ticks;
```
Then we want to initialize these variables to make the processes have initially one ticket. Any new process is allocated through the `allocproc` function, So go to `proc.c` and in the allocproc we see that it scan the process table to locate an empty slot which will be holding `UNUSED` label and if found it will jump to the found label to initialize some variables such as pid and so on, in this section we initialize our variables also
```
p->tickets = 1;
p->ticks = 0;
```
Like and ordinary system call we will change Five files: </br>
- syscall.h
- syscall.c
- sysproc.c
- usys.S
- user.h

In `syscall.h` There is a number assigned to every system call. And there is initially 21 of them already defined </br>
Add the following line with the appropriate number

>    #define SYS_settickets    XX

In `syscall.c` Add a pointer to the system call </br>
this file contains array of function pointer which use the number we assigned in syscall.h as a pointer to the system call which will be defined in differen file. so add this line in its appropriate position:</br>

>    [SYS_settickets]   sys_settickets

This means, when system call occurred with system call number XX, function pointed by function pointer sys_getreadcount will be called.</br>
Also in this file is add the function prototype so as to be able to define it in different place. So add this line </br>

>    extern int sys_settickets(void)

In `sysproc.c` we implement the system call function.</br>
```
int
sys_settickets(void)
{
  int num_tickets;
  if (argint(0, &num_tickets) < 0)
    return -1;
  
  settickets(num_tickets);
  return 0;
}
```
In `usys.S` we add interface to make the userprogram able to call the system call </br>

>    SYSCALL(settickets)

Finally in `user.h` we add the function which will be called from the user program

>    int settickets(void);

#### to do code to generate the random number 

#### In `proc.c` at the scheduler function count the total number of tickets for all runnable processes
```
for(total_tickets = 0, p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      if(p->state==RUNNABLE)
        total_tickets+=p->tickets;
```


#### Perform the randomized Lottery.</br>
using random at most function to generate number between 0 and total_tickets</br>

>	winner = random_at_most(total_tickets);

#### Determine which process owns this ticket
We can do this by using a counter starts from zero and accumelates the tickets every iteration until we hit the range of the winner process when counter value is greater than the winner value</br>
this section inside the function might look like this
```
...
for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE) {
            continue;
      }

      // looking for the winner range
      counter += p->tickets;

      if (counter < winner) {
            continue;
      }

      p->ticks += 1;
      
      ...
```
After that we should break from the loop to not execute the process next to the current process and make the sceduler start the whole process again.


## One last thing to do is handling the fork system call
we need to make the child process has the same tickets as the parent, so it's an easy job we can perform it by adding the following line to the fork function in the appropriate place:

>	 np->tickets = curproc->tickets;

this line will make the child inherit the parent tickets.


### Test

---
## Virtual memory
### Required

Read-only code:
- change the protection of parts of the page table to be read-only or read/write
- xv6 should trap and kill the process if the code tries to access a protected page.
- the page protection should be inherited on `fork()` . Thus, if a process has mprotected some of its pages, when the process calls fork, the OS should copy those protections to the child process.
- if any of these happened : `return -1` and do not change anything. otherwise, `return 0` upon success.
    > `addr` is not page aligned

    > `addr` points to a region that is not currently a part of the address space

    > `len` is less than or equal to zero.


### Implementation

Read-only code:
- by changing the `WRITEABLE` protection bit in the page table entry we control its write protection
- to do this we created 2 system calls `int mprotect(void *addr,int len)` and `int munprotect(void *addr,int len)`.
    - `mprotect` change the protection of the page to read only
    - `munprotect` change the protection of the page to read/write 
    - `addr` is the start of the page.
    - `len` is the number of pages to be modified.
- the system calles are done by modifing the following files:
    - `syscall.h` in this file you add the system call name in the following pattern `SYS_name` and give it the mex of the numbers in the file. this will be used as an index in the array of functions in `syscall.c` and will be used by the trap frame to know which function to be executed when a system call occures.
        ```c
        #define SYS_mprotect 25
        #define SYS_munprotect 26
        ```

    - `syscall.c` in this file you make the systemcall prototype and make it extern to be used outside this file in the following pattern `sys_name(void);`  and add them to the array `syscalls`
        ```c
        extern int sys_mprotect(void);
        extern int sys_munprotect(void);
        ```
        ```c
        [SYS_mprotect] sys_mprotect,
        [SYS_munprotect] sys_munprotect,
        ```

    - `defs.h`
        ```c
        int mprotect(void *addr,uint len);
        int munprotect(void *addr,uint len);
        ```
    - `usys.S`
        ```c
        SYSCALL(mprotect)
        SYSCALL(munprotect)
        ```
    - `sysproc.c` in this file you implement `sys_mprotect` and `sys_munprotect` . it's purpose is to get the arguments from the stack and check if they are valid before passing them to the `mprotect` and `munprotect`

        ```c
        int sys_mprotect(void){
            void *addr;
            int len;
            if(argptr(0,(void*)&addr,sizeof(void*))<0||argint(1,&len)<0)return -1; 
            if(len <= 0){
                cprintf("\nzero/negative length!\n");
                return -1;
            }
            if((int)(((int)addr)%PGSIZE)){
                cprintf("\nnot page aligned!\n");
                return -1;
            }
            return mprotect(addr,len);
        }
        ```
        ```c
        int sys_munprotect(void){
            void * addr;
            int len;
            if(argptr(0,(void*)&addr,sizeof(void*))<0||argint(1,&len)<0)return -1; 
            if(len <= 0){
                cprintf("\nzero/negative length!\n");
                return -1;
            }
            if((int)(((int)addr)%PGSIZE)){
                cprintf("\nnot page aligned!\n");
                return -1;
            }
            return munprotect(addr,len);
        }
        ```
    - `vm.c` implement the systemcalls `mprotect` , `munprotect` . 
        - we iterate over all the pages that need to be protected and get their page table entry with the help of the function `walkpgdir` and change the flag bit of `writable` if the current page is present and available for user mode and if not the function will return -1; uppon unsuccessfull changes.
        - we reset the regester cr3 at the end just to notify the hardware with the changes happened to the page table entries. [see Figure 2-1 for more clarification on `walkpgdir` & protection bits](https://pekopeko11.sakura.ne.jp/unix_v6/xv6-book/en/Page_tables.html#paging-hardware).
        - cr3 stores current physical address of the page directory thats why we used `V2P()` to change from virtual address to physical one . [see Figure 2-2 for more clarification on `V2P()` & `P2V()`](https://pekopeko11.sakura.ne.jp/unix_v6/xv6-book/en/Page_tables.html#process-address-space).
            >  PhysicalAddress = VirtualAddress - KERNELBASE;
        ```c
        int mprotect(void* addr,uint len){
            struct proc *curproc = myproc();  
            pte_t *pte;
            for(int i=(int)addr;i<((int)addr+len*PGSIZE);i+=PGSIZE){
                pte = walkpgdir(curproc->pgdir,(void*)i,0);
                if((*pte & PTE_U) && (*pte&PTE_P))*pte &= ~PTE_W;
                else return -1;
            }
            lcr3(V2P((uint)curproc->pgdir));
            return 0;
        }
        ```
        ```c
        int munprotect(void* addr,uint len){
            struct proc *curproc = myproc();  
            pte_t *pte;
            for(int i=(int)addr;i<((int)addr+len*PGSIZE);i+=PGSIZE){
                pte = walkpgdir(curproc->pgdir,(void*)i,0);
                if((*pte & PTE_U) && (*pte&PTE_P))*pte |= PTE_W;
                else return -1;
            }
            lcr3(V2P((uint)curproc->pgdir));
            return 0;
        }
        ```

### Test

Read-only code:
- `pro_test1.c` : tests if `mprotect` & `munprotect` is working 
- `pro_test2.c` : tests if `len` is working 'working on 2 pages'
- `pro_test3.c` : tests if the child process inhereted the protection from the parent process
- `pro_test4.c` : tests the validation of the `addr` & `len`
    - pro_test4 offset len
    - offset will be added to addr
    - ex :  pro_test4 -54 35  
        - `addr`+= -54;
        - `len` = 35;
---
## Threads
### Required

### Implementation

### Test

---
## References
- [add new system call]
- [how does system call works]
- [context switch]
- [memory managment]
- [xv6 book]
- [allocating memory]
- [sys calls]

## Team
- Mohamed Fathy Salah

[add new system call]: <https://medium.com/@viduniwickramarachchi/add-a-new-system-call-in-xv6-5486c2437573>
[how does system call works]: <https://medium.com/@flag_seeker/xv6-system-calls-how-it-works-c541408f21ff>
[context switch]: <https://ppan-brian.medium.com/context-switch-from-xv6-aedcb1246cd>
[memory managment]: <https://ppan-brian.medium.com/xv6-memory-management-3dfc1d53bd4f>
[xv6 book]: <https://pekopeko11.sakura.ne.jp/unix_v6/xv6-book/en/index.html>
[allocating memory]: <http://people.cs.pitt.edu/~lol16/CS1550_fall18/data/CS1550_Lab3.pdf>
[sys calls]: <https://github.com/YehudaShapira/xv6-explained/blob/master/xv6%20Code%20Explained.md>

