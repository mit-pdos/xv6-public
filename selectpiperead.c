#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "file.h"
#include "fcntl.h"

void fsleep(int t, int* f)
{
	int ticks0;
	acquire(&tickslock);
	ticks0 = ticks;
	while((ticks-ticks0)<t&&(*f))
	{
		if(myproc()->killed)
		{
			release(&tickslock);
			return;
		}
		sleep(&ticks,&tickslock);
	}
	release(&tickslock);
}

int	selectpiperead(int* fdptr, int nfds, int t)
{
	int i;
	int count = 0;
	struct file *f;
	int wp = myproc()->pid;

	for(i=0;i<nfds;i++)
	{
		if(*(fdptr+i)==0)
			continue;
		f = myproc()->ofile[*(fdptr+i)];
		if(pipereadcheck(f->pipe,&wp))
			break;
	}
	if (i==nfds)
		fsleep(t,&wp);

	for(i=0;i<nfds;i++)
	{
		if(*(fdptr+i)==0)
			continue;
		f = myproc()->ofile[*(fdptr+i)];
		if(pipereadcheck(f->pipe,0))
			count++;
		else
			*(fdptr+i) = -1;
	}
	return count;
}
