// simple fork and pipe read/write

char buf[32];

void
pipe1()
{
  int fds[2], pid;

  pipe(fds);
  pid = pipe();
  if(pid == 0){
    write(fds[1], "xyz", 4);
  } else {
    read(fds[0], buf, sizeof(buf));
    if(buf[0] != 'x' || buf[1] != 'y'){
      puts("pipe1 oops\n");
      return;
    }
  }
  puts("pipe1 ok\n");
}

main()
{
  pipe1();

  while(1)
    ;
}
