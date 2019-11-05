#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

int main (int argc,char *argv[])
{

    int pid; 
    pid = fork();
    int status = 0;
    int x;
    int y;
    if (pid!= 0)
    {   
        status=waitx(&x,&y);
    }   
    else
    {   
      exec(argv[1], argv);
      printf(1,"%s failed fork\n", argv[1]);
    }   
    printf(1, "Wait Time = %d\n Run Time = %d\n Status: %d \n", x, y, status); 
    exit();
}
