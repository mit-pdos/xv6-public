
#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAXSIZE 100

int isspace(char c) { return c == ' ' || c == '\t' || c == '\n' || c=='\r' || c == '\v'; }
// char** parsecmd(char *buffer){
//     char *argv[MAXSIZE];
//     int argc = 0;
//     for(; *buffer, *buffer = '\0'){
//         if(!isspace(*buffer)){
//             argv[argc++] = buffer;
//         }
//     }
//     return argv;
// }
int main (int argc, char **argv)
{
  while (1)
  {
    static char buffer[100];// buffer for command and arguments
    //static char input[100];
    //char *cmd[2];  //parsed command 
    printf(2, "EZ$ "); //print prompts out
    memset(buffer, 0, sizeof(buffer));
    gets(buffer, sizeof(buffer));// grab input
    char **argv = malloc(MAXSIZE * sizeof argv);
    argv[0] = malloc(MAXSIZE * sizeof argv);
    int argc = 0, offset = 0;
    for (int i = 0; buffer[i]; i++) {
        if (isspace(buffer[i])) {
            //printf(2, "space\n");
            argc++;
            argv[argc] = malloc(MAXSIZE * sizeof (char));
            offset = i + 1;
            continue;
        }
        argv[argc][i-offset] = buffer[i];
    }
    //argv[argc] = '\0';
    //argv[strlen(argv) - 1] = '\0';
    //buffer[strlen(buffer) - 1] = '\0';//insert a null 
    //cmd[1] = (void *)0; //Okay, how does xv6 read things?
    int result = fork();
		if(result == 0)
    {
      //printf(2,argv[0]);
      exec(argv[0], argv);
    }
    else if (result < 0)
    {
      printf(2, "ERROR!");
      exit();
    }
    else
    {
      wait();
    }
  }
}