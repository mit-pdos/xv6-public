#include "types.h"
#include "stat.h"
#include "user.h"

void fork_(int i, int fd1, int fd2)
{
	int pid;
	if((pid=fork())==0)
	{
		char buf[4] = {0};
		buf[0] = '0'+i;
		buf[3] = '\n';
		write(fd2,buf,1);
		while(read(fd1,buf+2,1))
		{
			if(buf[2]=='0')
				exit();
			write(1,buf,4);
			write(fd2,buf,1);
		}
	}
}

int main()
{
	int fork_num = 3;
	int n;
	int fd[fork_num*2][2];
	char c='a'-1,buf;
	for(int i=0;i<fork_num;i++)
	{
		pipe(fd[i]);
		pipe(fd[fork_num+i]);
		fork_(i+1,fd[i+fork_num][0],fd[i][1]);
	}
	int *fdsptr = malloc(sizeof(int)*fork_num);
	while(c<'z')
	{
		for(int i=0;i<fork_num;i++)
			fdsptr[i] = fd[i][0];
		n = selectpiperead(fdsptr,fork_num,100000);
		if(n>0)
		{
			for(int i=0;i<fork_num;i++)
			{
				if(fdsptr[i]>=0)
				{
					read(fdsptr[i],&buf,1);
					if(c<'z')
						c++;
					write(fd[i+fork_num][1],&c,1);
				}
			}
		}
	}
	for(int i=0;i<fork_num;i++)
	{
		c = '0';
		write(fd[i+fork_num][1],&c,1);
		wait();
	}
	return 0;
}
