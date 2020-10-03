# Project 4: Intro to Kernel Hacking
#### Meg Jaffy | CS 372 | September 23, 2020

## Part 1: System Call Tracing
To implement system call tracing, I only made edits in the file "syscall.c". There is an integer variable in the syscall(void) function called ``print``. To turn off system tracing, simply set this value to 0. I defined a function ``callName(num)`` which returns the name of the system call associated with ``num``. Inside  ``syscall()`` if print is true, I use cprintf to print out the name given by ``callName(num)``, followed by an arrow and then the value in  ``curproc->tf->eax`` which is where the return value from the system call was stored. So, the name of the system call followed by the return value are printed if ``print`` is true.  

## Part 2: Date System Call
First, I defined `sys_date` as 22 in "syscall.h", then added the function `sys_date` in "sycall.c" as an external function, and added it to the array of functions. I also added an entry for `SYS_date` in my function callName, so that it will work with system tracing. I also added a definition for `date` in "user.h", but this time with an input of a `struct rtcdate` pointer as an input, rather than voiding the input. Additionally, I included the new syscall in "usys.h".

With all of this in place, I was able to add the actual system call (`sys_date(void)`) definition to "sysproc.c". First, I define an integer `r` and then use `argint` to assign the first argument to this integer -- however since the first argument is actually a pointer to an `rtcdate` object, I cast `r` to this pointer before using it. I acquire the `tickslock` lock before doing anything with this struct, so that there will not be any concurrency issues. Then, I use `cmostime` to fill the pointer `r` with the current date. Since this is the pointer given as an argument to the function, the user program will have access to whatever value is filled in here. Finally, I release the lock and return `0`.  
