#include "types.h"
#include "user.h"

#define WNOHANG 	1

int main(int argc, char *argv[])
{
	
	int exitWait(void);
	int waitPid(void);

  printf(1, "\n This program tests the correctness of your lab#1\n");
  
  if (atoi(argv[1]) == 1)
	exitWait();
  else if (atoi(argv[1]) == 2)
	waitPid();
  else 
   printf(1, "\ntype \"%s 1\" to test exit and wait, \"%s 2\" to test waitpid\n", argv[0], argv[0]);
  
    // End of test
	 exit(0);
	 return 0;
 }
  
int exitWait(void) {
	  int pid, ret_pid, exit_status;
    int i;
  // use this part to test exit(int status) and wait(int* status)
 
  printf(1, "\n  Parts a & b) testing exit(int status) and wait(int* status):\n");

  for (i = 0; i < 2; i++) {
    pid = fork();
    if (pid == 0) { // only the child executed this code
      if (i == 0){
        printf(1, "\nThis is child with PID# %d and I will exit with status %d\n", getpid(), 0);
        exit(0);
      }
      else{
	      printf(1, "\nThis is child with PID# %d and I will exit with status %d\n" ,getpid(), -1);
        exit(-1);
      } 
    } else if (pid > 0) { // only the parent executes this code
      ret_pid = wait(&exit_status);
      printf(1, "\n This is the parent: child with PID# %d has exited with status %d\n", ret_pid, exit_status);
    } else { // something went wrong with fork system call
	    printf(2, "\nError using fork\n");
      exit(-1);
    }
  }
  return 0;
}

int waitPid(void){
	
  int ret_pid, exit_status;
  int i;
  int pid_a[5]={0, 0, 0, 0, 0};
 // use this part to test wait(int pid, int* status, int options)

  printf(1, "\n  Part c) testing waitpid(int pid, int* status, int options):\n");

	for (i = 0; i <5; i++) {
		pid_a[i] = fork();
		if (pid_a[i] == 0) { // only the child executed this code
			printf(1, "\n This is child with PID# %d and I will exit with status %d\n", getpid(), getpid() + 4);
			exit(getpid() + 4);
		}
	}
  sleep(5);
  printf(1, "\n This is the parent: Now waiting for child with PID# %d\n",pid_a[3]);
  ret_pid = waitpid(pid_a[3], &exit_status, 0);
  printf(1, "\n This is the partent: Child# %d has exited with status %d, expected: %d\n",ret_pid, exit_status, pid_a[3] + 4);
  sleep(5);
  printf(1, "\n This is the parent: Now waiting for child with PID# %d\n",pid_a[1]);
  ret_pid = waitpid(pid_a[1], &exit_status, 0);
  printf(1, "\n This is the parent: Child# %d has exited with status %d, expected: %d\n",ret_pid, exit_status, pid_a[1] + 4);
  sleep(5);
  printf(1, "\n This is the parent: Now waiting for child with PID# %d\n",pid_a[2]);
  ret_pid = waitpid(pid_a[2], &exit_status, 0);
  printf(1, "\n This is the partent: Child# %d has exited with status %d, expected: %d\n",ret_pid, exit_status, pid_a[2] + 4);
  sleep(5);
  printf(1, "\n This is the parent: Now waiting for child with PID# %d\n",pid_a[0]);
  ret_pid = waitpid(pid_a[0], &exit_status, 0);
  printf(1, "\n This is the partent: Child# %d has exited with status %d, expected: %d\n",ret_pid, exit_status, pid_a[0] + 4);
  sleep(5);
  printf(1, "\n This is the parent: Now waiting for child with PID# %d\n",pid_a[4]);
  ret_pid = waitpid(pid_a[4], &exit_status, 0);
  printf(1, "\n This is the parent: Child# %d has exited with status %d, expected: %d\n",ret_pid, exit_status, pid_a[4] + 4);


  printf(1, "\n This is the parent: Now try to wait for a invalid Number, this should not get stuck..\n");
  ret_pid = waitpid(9999, &exit_status, 0);
  printf(1, "\n This is the parent: Child# 9999 has ret code %d, expected: -1\n",ret_pid);
  //printf(1, "\n This is the parent: Child# %d has exited with status %d\n",ret_pid, exit_status);

  printf(1, "\n This is the parent: Now try to give invalid argument.\n");
  ret_pid = waitpid(9999, (int*) 0xffffffff, 0);
  printf(1, "\n This is the parent: Got ret code %d, expected: -1\n",ret_pid);

  return 0;
}
