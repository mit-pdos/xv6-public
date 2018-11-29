void doNothingHandler(void)
{
	;
}

void sigIntHandler(void)
{
	release(&ptable.lock);
	kill(myproc()->pid);
	acquire(&ptable.lock);
}

void sigUsrHandler(void)
{
	cprintf("SIGUSR1 %d",myproc()->pid);
}

void sigUer2Handler(void)
{
	cprintf("SIGUSR2 %d",myproc()->pid);
}

void sigChldHandler(void)
{
	;
}
