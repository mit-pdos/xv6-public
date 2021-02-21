
#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAXSIZE 100

int isspace(char c) { return c == ' ' || c == '\t' || c == '\n' || c=='\r' || c == '\v'; }
void cd(char **argv){
    if(chdir(argv[1]) != 0){
        printf(2,"Directory not found");
    }
}
int main (int otherstuff, char **stuff)
{
  char*** history = malloc(MAXSIZE * sizeof(char **));
  int historyi = 0;
  while (1)
  {
    static char buffer[100];// buffer for command and arguments
    //static char input[100];
    //char *cmd[2];  //parsed command 
    printf(2, "[%d]EZ$ ", historyi); //print prompts out
    memset(buffer, 0, sizeof(buffer));
    gets(buffer, sizeof(buffer));// grab input
    //parse magic
    char **argv = malloc(MAXSIZE * sizeof argv);
    argv[0] = malloc(MAXSIZE * sizeof argv);
    int argc = 0, beginning = 0;
    for (int i = 0; buffer[i]; i++) {
        if (isspace(buffer[i])) {
            //printf(2, "space\n");
            argc++;
            argv[argc] = malloc(MAXSIZE * sizeof (char));
            beginning = i + 1; //create index to beginning of next argument
            
        }
        else{
            argv[argc][i-beginning] = buffer[i];
        }
    }
    for(int i = argc; i < MAXSIZE; i++)
        argv[i] = '\0';
    if((strcmp(argv[0], "cd") == 0)){
        cd(argv);
        history[historyi] = argv;
    }
    else if ((strcmp(argv[0], "chdir") == 0)){
        cd(argv);
        history[historyi] = argv;
    }
    else if(argv[0][0] == '#'){
        argv = history[atoi(argv[0]+1)];
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
        history[historyi] = argv;
    }
    else{
        history[historyi] = argv;
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
    if(historyi == MAXSIZE-1){
        historyi = 0;
    }
    else{
        historyi++;
    }
  }
}