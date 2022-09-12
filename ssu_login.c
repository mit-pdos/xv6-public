#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h" 

char userID[16][32];
char pwdID[16][32];

int cnt = 0;
int isEmpty = 0;

void parse(int fd, char *array)
{
  for (int i = 0;; i++)
  {
    if (read(fd, &array[i], 1) <= 0) {
      isEmpty = 1;
      break;
    }
    if (array[i] == ' ' || array[i] == '\n') {
      array[i] = '\0';
      break;
    }
  }
}

void get_user_list()
{
  int fd;

  fd = open("list.txt", O_RDONLY);
  if (fd < 0)
  {
    printf(1, "list.txt open error\n");
    exit();
  }

  for (int i = 0; i < 16; i++)
  {
    if(isEmpty)
      break;
    cnt++;
    parse(fd, userID[i]);
    parse(fd, pwdID[i]);
  }
}

int check_idpw()
{
  char id[32]={0};
  char pw[32] = {0};

  printf(1, "Username: ");
  parse(0, id); 
  printf(1, "Password: ");
  parse(0, pw);

  for (int i = 0; i < cnt; i++)
  {
    if (strcmp(id, userID[i])==0)
    {
      if (strcmp(pw, pwdID[i])==0)
      {
        return 1;
      }
      return 0;
    }
  }
  return 0;
}

int main(int argc, char *argv[])
{
  int pid, wpid;
  get_user_list();

  while(1)
  {
    if (check_idpw()){
      while(1)
      {
        printf(1, "init: starting sh\n");
        pid = fork();
        if (pid < 0)
        {
          printf(1, "init: fork failed\n");
          exit();
        }
        if (pid == 0)
        {
          exec("sh", argv);
          printf(1, "init: exec sh failed\n");
          exit();
        }
        while ((wpid = wait()) >= 0 && wpid != pid)
          printf(1, "zombie!\n");
      }
    }
  }
}
