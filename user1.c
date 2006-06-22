void
fork()
{
  asm("mov $1, %eax");
  asm("int $48");
}

main()
{
  fork();
  while(1)
    ;
}
