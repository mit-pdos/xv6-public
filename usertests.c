char buf[2048];

// simple fork and pipe read/write

void
pipe1()
{
  int fds[2], pid;
  int seq = 0, i, n, cc, total;

  pipe(fds);
  pid = fork();
  if(pid == 0){
    close(fds[0]);
    for(n = 0; n < 5; n++){
      for(i = 0; i < 1033; i++)
        buf[i] = seq++;
      if(write(fds[1], buf, 1033) != 1033){
        puts("pipe1 oops 1\n");
        exit(1);
      }
    }
    exit(0);
  } else {
    close(fds[1]);
    total = 0;
    cc = 1;
    while(1){
      n = read(fds[0], buf, cc);
      if(n < 1)
        break;
      for(i = 0; i < n; i++){
        if((buf[i] & 0xff) != (seq++ & 0xff)){
          puts("pipe1 oops 2\n");
          return;
        }
      }
      total += n;
      cc = cc * 2;
      if(cc > sizeof(buf))
        cc = sizeof(buf);
    }
    if(total != 5 * 1033)
      puts("pipe1 oops 3\n");
    close(fds[0]);
  }
  puts("pipe1 ok\n");
}

// meant to be run w/ at most two CPUs
void
preempt()
{
  int pid1, pid2, pid3;
  int pfds[2];

  pid1 = fork();
  if(pid1 == 0)
    while(1)
      ;
    
  pid2 = fork();
  if(pid2 == 0)
    while(1)
      ;

  pipe(pfds);
  pid3 = fork();
  if(pid3 == 0){
    close(pfds[0]);
    if(write(pfds[1], "x", 1) != 1)
      puts("preempt write error");
    close(pfds[1]);
    while(1)
      ;
  }

  close(pfds[1]);
  if(read(pfds[0], buf, sizeof(buf)) != 1){
    puts("preempt read error");
    return;
  }
  close(pfds[0]);
  kill(pid1);
  kill(pid2);
  kill(pid3);
  wait();
  wait();
  wait();
  puts("preempt ok\n");
}

main()
{
  puts("usertests starting\n");
  //pipe1();
  preempt();

  while(1)
    ;
}
