// simple fork and pipe read/write

char buf[2048];

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

main()
{
  pipe1();

  while(1)
    ;
}
