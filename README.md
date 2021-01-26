# Table of content
- [Scheduler](#Scheduler)
    - [Required](#Required)
    - [Implementation](#Implementation)
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

## Required
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

## Implementation
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
```c
p->tickets = 1;
p->ticks = 0;
```
Like any ordinary system call we will change Five files: </br>
- syscall.h
- syscall.c
- sysproc.c
- usys.S
- user.h

In `syscall.h` There is a number assigned to every system call. And there is initially 21 of them already defined </br>
Add the following line with the appropriate number
```c
#define SYS_settickets    XX
```
In `syscall.c` Add a pointer to the system call </br>
this file contains array of function pointer which use the number we assigned in syscall.h as a pointer to the system call which will be defined in differen file. so add this line in its appropriate position:</br>
```c
[SYS_settickets]   sys_settickets
```
This means, when system call occurred with system call number XX, function pointed by function pointer sys_getreadcount will be called.</br>
Also in this file is add the function prototype so as to be able to define it in different place. So add this line </br>
```c
extern int sys_settickets(void)
```
In `sysproc.c` we implement the system call function.</br>
```c
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
```c
SYSCALL(settickets)
```
Finally in `user.h` we add the function which will be called from the user program
```c
int settickets(void);
```
#### code to generate the random number 

This part is done using a free open source library `rand.h` which implemented in `rand.c` it has three main functions for doing this job two of them are helper functions
which are `genrand()` and `sgenrand()` but the one we are interested in is `random_at_most(int max_num)` which will generate the number between 1 and the max_num.</br>
- `sgenrand()` setting the initial seeds into an array, so this function is called once in another function called genrand().
- `genrand()` generates pseudo random real number in the interval [0-1] for each call. so every time we call random_at_most() this function is called.
- `random_at_most(int max_num)` Returns number in the interval [0, max_num] 


#### In `proc.c` at the scheduler function count the total number of tickets for all runnable processes
```c
for(total_tickets = 0, p = ptable.proc; p < &ptable.proc[NPROC]; p++)
      if(p->state==RUNNABLE)
        total_tickets+=p->tickets;
```


#### Perform the randomized Lottery.</br>
using random at most function to generate number between 0 and total_tickets</br>
```c
winner = random_at_most(total_tickets);
```
#### Determine which process owns this ticket
We can do this by using a counter starts from zero and accumelates the tickets every iteration until we hit the range of the winner process when counter value is greater than the winner value</br>
this section inside the function might look like this
```c
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
```c
np->tickets = curproc->tickets;
```
this line will make the child inherit the parent tickets.


### Test

---

## Virtual memory
### Required
<h1 align="center">Null pointer dereference</h1>
In this part of the project we will be changing the xv6 to support a feature almost every modern os does, which is causing an exception when a program dereferences a null pointer <br>
what is null pointer:<br>
A Null Pointer is a pointer that does not point to any memory location. It stores the base address of the segment. The null pointer basically stores the Null value <br>
what is null pointer dereference:<br>
null pointer Dereferencing  means trying to access whatever is pointed to by the pointer. The * operator is the dereferencing operator
in other words Dereferencing just means reading the memory value at a given address. So when you have a pointer points to something, to dereference the pointer means to read or write the data that the pointer points to.

In Linux trying to dereference a null pointer will cause an exception
![example-Linux](https://i.ibb.co/ZJYSkKZ/linux-Nullpointer-Dereference.png "linux") <br>
In xv6 trying to dereference a null pointer will not cause an exception ,however, it gave us a Trap 6 error <br>
![example-xv6](https://i.ibb.co/chfR346/Screenshot-from-2021-01-25-16-21-02.png "xv6-try1") <br>
this could be fixed by updating CFLAGS in Makefile <br>
Now we can see clearly trying to dereference a null pointer in xv6 will not cause an exception
![example-xv6](https://i.ibb.co/PrbSLR0/Screenshot-from-2021-01-25-16-29-30.png "xv6-try12") <br>
Now we will try to make the xv6 support this feature and make an exception when trying to dereferencing a null pointer


<h1 align="center">Read-only code</h1>

- change the protection of parts of the page table to be read-only or read/write
- xv6 should trap and kill the process if the code tries to access a protected page.
- the page protection should be inherited on `fork()` . Thus, if a process has mprotected some of its pages, when the process calls fork, the OS should copy those protections to the child process.
- if any of these happened : `return -1` and do not change anything. otherwise, `return 0` upon success.
    > `addr` is not page aligned

    > `addr` points to a region that is not currently a part of the address space

    > `len` is less than or equal to zero.


### Implementation
Null pointer dereference:<br>
the basic idea to make the xv6 supports null pointer exception is to make the user program loads into memory from the second page which in address 4096 or 0x1000H not from the first page with adress 0x0000  i.e we have to make page 0 is inaccessible
1. change `sz = 0` to `sz = PGSIZE` `exec.c`
 ```c
 sz= PGSIZE
 ```
`exec` func is used to replace current process with new one in the part of looping over every section in `proghdr` we need to allocate memory sothat we will force the program to start from second page i.e `sz=4096`

2. Change `i=0` to `i=4096` in func `copyuvm` in `vm.c`

 in `proc.c` we have a fun called `fork` that is used to create a new process by copying from parent.
 in `fork` func we use anothe func called `copyuvm` defined in `vm.c` given a parent process page table it creates a copy from it to the child process
 in our case when the `copyuvm` loops over all pages in the page table we have to force it to start from the second page i.e from 4096 instead of 0 as we did
  ```c
 for (i = 4096; i < sz; i += PGSIZE)
 ```
3. adding one more condition which is `i==0` in func `argptr` in syscall .c 
  ```c
  if (size < 0 || (uint)i >= curproc->sz || (uint)i + size > curproc->sz || i == 0)
 ```
  when we pass a pointer into the kernel,the kernal must check it is not a bad pointer (null pointer) so that we added one more condition `i==0` to make sure that we didn't pass a null pointer 
  
4. update MAKEFILE to set the start point is the second page `0x1000` not the first page `0x0000`
in line 149
```c
	$(LD) $(LDFLAGS) -N -e main -Ttext 0x1000 -o $@ $^
```
in line 156 
```c
	$(LD) $(LDFLAGS) -N -e main -Ttext 0x1000 -o _forktest forktest.o ulib.o usys.o
```
In xv6 makefile user programs are compiled , by default the entry point [first instruction] is 0 and the changes we have made in the previous 3 steps we made the first page is invalid so that we have to set the entry point from the second page

5. Last one is to change `p=0` into `p=4096` in func `validatetest` in `usertests.c`
```c
  for (p = 4096; p <= (uint)hi; p += 4096)
```
 To make the usertest pass


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
Null pointer dereference:
`null_test.c` : </br>
 1. create a pointer 
 2. initilize it to null
 3. try to dereference it 
```c
   int *point; 
   point = NULL; 
   printf(1, "Dereferencing a pointer now %d \n\n  ", *point); 
 ```

it will give trap 14 which is page fault error i.e we can't access page 0 and we made the program raise an exception while dereferencing a null pointer </br>
![null_test](https://i.ibb.co/pnLV6bX/Screenshot-from-2021-01-25-21-32-40.png "null pointer dereference") </br>



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
#### Required
Adding Kernel level support for user threads ,that means multiple tasks sharing the same memory can now be ran simultaneously .
### we will implement two system calls and library for threads:
-system calls:-</br>
1 - `clone` ->implement multiple threads-like processes of control in a program that run concurrently in a shared memory space.</br>
* creates a child process -behaves like a thread- that shares the address space and working directory of the parent -calling- process</br>
* if The child process calls clone() another thread in the same parent process will be created.</br>
* the child process's stack is in the address space of the parent process</br>
2 - `join` ->to wait for the thread to finish.</br></br>
-thread library implemented in "ulib.c" :-</br>
1 - `thread_create` ->allocate user stack by using sbrk or malloc ,then call the clone system call.</br>
2 - `thread_join` ->uses the join system call.</br>
3 - `lock init` ->intialize the lock by 0.</br>
4 - `lock_acquire` -> xchg the lock by 1 during the critical region.</br>
5 - `lock_release` ->xchg lock by 0 after finishing the critical region.</br>

### Implementation
#### 1- `clone()`
the function's full signature is : </br>
`int clone(void (*fcn)(void*, void*), void *arg1, void *arg2, void *stack)` </br>
It takes 4 arguments..
* `void (*fcn)(void*, void*)` a pointer to function `fcn`, which will be executed when the child process is created with clone()</br>
<a name="fargm"></a>
* `void *arg1, void *arg2` pointers to the two arguments passed to the funciton `fcn`, those arguments are accessed from the child processe's userstack `stack`</br>
* `void *stack` a pointer to a location in the parent process address space which contains the stack of the child process , The parent process sets up memory space for the child stack because the child and parent process share address space, it is not possible for the child process to execute in the same stack as the parent process</br>
*REMEMBER </br>
Stack grows downward, so `stack` usually points to the topmost address of the memory space allocated by the parent process for the child stackb but here it will poitnts to the bottom of the specified memory [SEE](#setstack).</br>
##### Modified files
###### `sysproc.c`
> provides wrapper functions to the raw system calls which makes sure the user has provided the right number and type of arguments before forwarding the arguments > to the actual system call. </br>

`sys_clone` takes no arguments as The arguments are accessed from the stack at known offsets from the %esp register using `argint` that retrieves the nth system call arguments </br>
```c
int
sys_clone(void)
{
int fcn , arg1 , arg2 ,stack;
// if `argint` value is -1, it means that it couldn't retrieves the nth argument of the `clone` then return -1
if(argint(0,&fcn)<0)return -1; 
if(argint(1,&arg1)<0)return -1;
if(argint(2,&arg2)<0)return -1;
if(argint(3,&stack)<0)return -1;
//if all the arguments were found pass those arguments to `clone()`
return clone((void *)fcn, (void *)arg1, (void *)arg2,(void *)stack); //
}
```
###### `proc.c`
> to declare the `clone()` function

1- set two `struct proc` pointers , one for the new created process and the other points to the parent process</br>
```c
int clone (void(*fcn)(void*,void*),void *arg1 ,void *arg2 ,void* stack)
{
struct proc *newp;  
struct proc *currp = myproc(); 
```
2- call `allocproc()` that searches process table for an unused process and sets its state to EMBRYO. if there was no unused process it will return 0 so `clone()` will fail to create a child process. </br>
3- after creating the child process it will ..</br>
* use the callin process page directory`pgdir`which holds the process's page table
* share the size `sz` of the address space of the calling process
* point to the same trapframe `tp` as the calling process
* the new process parent will be the calling process
* set the return address of the clone (which is stored in the eax register) to 0 ,as the new process is the child process so that its Pid will always be 0
```c
if((newp = allocproc())==0) return -1;  

//setting new process data
newp->pgdir = currp->pgdir;  
newp->sz = currp->sz;              
*newp->tf = *currp->tf;	      
newp->parent = currp; 
newp->tf->eax = 0 ;         
```
<a name="setstack"></a>
4- set the child process stack..</br>
* its stack is pagesized so inorder to reach the top of the stack we use an offset`stack_top` and set it to sum of stack bottom `stack` and the size of the stack `PGSIZE` 
* declare an array of 3 elements as the child process needs 3 essential values on its stack frame once it starts
  * return address: the 1st value in the stack frame which the base poiner register`ebp` will be set to. this address is address of the previous ebp and is used to go back to the previous stack frame once the current function returns...`0xffffffff` is fake return address because there is no previous base pointer for any other stack frames as well as if the function never return,a system call exit() is always called at the end of the function which terminates the porgram
  * function arguments
    * `user_stack[1]= (uint) arg1` 1st value after the base pointer.
    * `user_stack[2]= (uint) arg2` 2nd value after the base pointer,as mentioned earliear [SEE](#fargm) function arguments are placed in order on the stack.So, the cloned function `fcn` will retrieve arg1 as its first argument.</br>
*REMEMBER </br>
stack is composed of 1 or many several stack frames. Each stack frame corresponds to a call to a function which has not yet terminated with a return</br>
A stack frame is a frame of data that gets pushed onto the stack.the function return address then the arguments and space for local variables are pushed onto the stack. Together, they make the "stack frame", the area between base pointer and stack pointer</br>
* subtract 12 bytes from the `stack_top` to make space for the 3 values being saved, each value represents 4 bytes
* `copyout()` is used to copy 12 bytes from the arry `user_stack` to the real stack in the `newp->pgdir`starting from memory location `stack_top`</br>
```c
uint stack_top = (uint) stack + PGSIZE;
uint user_stack[3]; 

user_stack[0]= 0xffffffff ;  
user_stack[1]= (uint) arg1 ; 
user_stack[2]= (uint) arg2 ; 
stack_top-=12; 

if(copyout(newp->pgdir , stack_top , user_stack ,12) < 0) return -1;
```
5- set base `ebp`and stack `esp`pointers for the return from trap</br>
 *REMEMBER</br>
 trap frame stores the state of the processes' registers</br>
 A process manages its stack using a base pointer`ebp`and stack pointer`esp`</br>
 A base pointer`ebp`: stores the address of the top of the current stack frame, this address is the address of the previous base, and is used to go back to the previous stack frame once the current function returns,all local variables and function parameters are at known constant offset from this register for the duration of the function call</br>
 A stack pointer`esp`: stores the lowest address the stack has reached as it changes during the function call when other functions are called. always points to the top of the stack.The child process will use `esp` to find the function args passed to it, and start execution from there.</br>
 * HERE the top of the current stack `esp`and stack frame `ebp` is`stack_top`, as we are returning into a function, Normally, `ebp` and `esp` won't be the same but this is a special case because we are starting a new process</br>
 * instruction pointer register`eip`is set to `fcn` to ensure that the child process will run `fcn` on start.
 * saves the address of the stack to `threadstack`
```c
newp->tf->ebp = (uint) stack_top;
newp->tf->esp = (uint) stack_top;
newp->tf->eip = (uint) fcn;
newp->threadstack=stack;
```
6-duplicate files
* duplicate all the opened files by the parent process to be opened by the child process, each process has about 14 openfiles,`filedup` is used to increment the reference count of the open files of the process 
* duplicate the current working directory `cwd` of the parent process to be opened by the child process
* set the name of new process as same the name of currnet process using `safestrcpy`
  *`safestrcpy` specifies the size of the destination `newp->name` to be equal to the size of source `currp->name` 
```c

for(int i=0 ; i<NOFILE ; i++) //looping over 14 openfile of process
	if(currp->ofile[i])  newp->ofile[i] = filedup(currp->ofile[i]); 

//increment reference count of cwd (crnt directry) and save it in newp->cwd
newp->cwd =idup(currp->cwd); 


safestrcpy(newp->name ,currp->name ,sizeof(currp->name));

acquire(&ptable.lock);  //make the lock =1
newp->state = RUNNABLE;
release(&ptable.lock);  //release spinlock from being held by cpu
return newp->pid;

}
```

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

