#include "pouch.h"
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "ns_types.h"
#include "param.h"

/*
*   Helper functions
*/
static void empty_string(char * string, int length)
{
    for (int i = 0; i < length; i++)
        string[i] = 0;
}

void
panic(char *s)
{
  printf(stderr, "%s\n", s);
  exit(1);
}

/*
*   Pouch functions
*/
static int pouch_limit_cgroup(char* container_name, char* cgroup_state_obj, char * limitation){

    char cg_limit_cname[256];

    prepare_cgroup_cname(container_name,cg_limit_cname);
    strcat(cg_limit_cname, "/");
    strcat(cg_limit_cname,cgroup_state_obj);

    int cont_fd = open(container_name, 0);
    if(cont_fd < 0){
       printf(stderr, "There is no container: %s in a started stage\n", container_name);
       exit(1);
    }
    int cname_fd = open(cg_limit_cname, O_RDWR);
    if(cname_fd < 0){
       printf(stderr, "Incorrect cgroup object-state provided. Not applied.\n", container_name);
       exit(1);
    }

    if(write(cname_fd, limitation, sizeof(limitation)) < 0)
       return -1;
    if(close(cname_fd) < 0)
       return -1;
    printf(1, "Pouch: %s cgroup applied \n",container_name);
    return 0;
}

static int prepare_cgroup_cname(char* container_name, char* cg_cname){
    strcpy(cg_cname,"/cgroup/");
    strcat(cg_cname, container_name);
    return 0;
}

static int pouch_cmd(char* container_name, enum p_cmd cmd){
        int tty_fd;
        int pid;
        char tty_name[10];
        char cg_cname[256];

        if(cmd == START){
           return pouch_fork(container_name);
        }

        if(cmd == LIST){
            if(print_clist() <0){
                return -1;
            }
            return 0;
        }

        if(read_from_cconf(container_name, tty_name, &pid) < 0){
           return -1;
        }

        if(cmd == INFO){
            if(print_cinfo(container_name, tty_name, pid) <0){
                return -1;
            }
            return 0;
        }

        if((tty_fd = open(tty_name, O_RDWR)) < 0){
           printf(stderr, "cannot open tty: %s\n",tty_name);
           return -1;
        }

        if(cmd == DESTROY && pid != 0){

            // Return the process to root cgroup.
            char cur_pid_buf[10];
            int cgroup_procs_fd = open("/cgroup/cgroup.procs", O_RDWR);
            itoa(cur_pid_buf, pid);
            if(write(cgroup_procs_fd, cur_pid_buf, sizeof(cur_pid_buf)) < 0)
                return -1;
            if(close(cgroup_procs_fd) < 0)
                return -1;

           if(kill(pid) < 0){
                return -1;
           }
           if(unlink(container_name) < 0)
               return -1;

           prepare_cgroup_cname(container_name,cg_cname);

           if(unlink(cg_cname) < 0){
               return -1;
           }
           if(remove_from_pconf(tty_name) < 0)
               return -1;

           if(is_connected_tty(tty_fd)){
               if(disconnect_tty(tty_fd) < 0)
                   return -1;
           }
           if(detach_tty(tty_fd) < 0)
               return -1;
           if(close(tty_fd) < 0)
               return -1;

           printf(1, "Pouch: %s destroyed\n",container_name);
           return 0;
        }



        if(cmd == CONNECT){
            if(!is_connected_tty(tty_fd)){
                if(connect_tty(tty_fd) < 0){
                     close(tty_fd);
                     printf(stderr, "cannot connect to the tty\n");
                     return -1;
                }
            }else{
                printf(1, "Pouch: %s is already connected\n",container_name);
            }

        }else if(cmd == DISCONNECT && disconnect_tty(tty_fd) < 0){
           close(tty_fd);
           printf(stderr, "cannot disconnect from tty\n");
           return -1;
        }

        close(tty_fd);
        return 0;
}

static int find_tty(char* tty_name){
        int i;
        int tty_fd;
        char tty[] = "/ttyX";

        // Not including the console tty
        for(i=0; i < (MAX_TTY - 1); i++){
            tty[4] = '0' + i;
            if((tty_fd = open(tty, O_RDWR)) < 0){
                printf(stderr, "cannot open %s fd\n", tty);
                return -1;
            }

            if(!is_attached_tty(tty_fd)){
                strcpy(tty_name,tty);
                close(tty_fd);
                return 0;
            }
            close(tty_fd);
        }

        return -1;
}

static int init_pouch_conf(){
    //create a file for eacth tty that will hold cname inside currently connected to it
    int i;
    int ttyc_fd;
    char ttyc[] = "tty.cX";

    // Not including the console tty
    for(i=0; i < (MAX_TTY - 1); i++){
        ttyc[5] = '0' + i;
        if((ttyc_fd = open(ttyc, O_CREATE|O_RDWR)) < 0){
            printf(stderr, "cannot open %s fd\n", ttyc);
            return -1;
        }
        if(close(ttyc_fd) < 0)
            return -1;
    }
    return 0;
}

static int write_to_pconf(char * ttyname, char * cname){
    char ttyc[] = "tty.cX";
    int ttyc_fd;
    ttyc[5] = ttyname[4];
    if((ttyc_fd = open(ttyc, O_CREATE | O_WRONLY)) < 0){
        printf(stderr, "cannot open %s fd\n", ttyc);
        return -1;
    }
    printf(ttyc_fd, "%s",cname);
    close(ttyc_fd);
    return 0;
}

static int remove_from_pconf(char * ttyname){
    char ttyc[] = "tty.cX";
    int ttyc_fd;
    ttyc[5] = ttyname[4];
    if((ttyc_fd = open(ttyc, O_RDWR)) < 0){
        printf(stderr, "cannot open %s fd\n", ttyc);
        return -1;
    }
    if(unlink(ttyc) < 0)
        return -1;
    if((ttyc_fd = open(ttyc, O_CREATE|O_RDWR)) < 0){
        printf(stderr, "cannot open %s fd\n", ttyc);
        return -1;
    }
    close(ttyc_fd);
    return 0;
}

static int read_from_pconf(char * ttyname, char * cname){
    char ttyc[] = "tty.cX";
    int ttyc_fd;
    ttyc[5] = ttyname[4];
    if((ttyc_fd = open(ttyc, O_RDWR)) < 0){
        printf(stderr, "cannot open %s fd\n", ttyc);
        return -1;
    }
    read(ttyc_fd, cname, CNTNAMESIZE);
    close(ttyc_fd);
    return 0;
}

static int print_clist(){
    int i;
    int tty_fd;
    char tty[] = "/ttyX";
    char buf[CNTNAMESIZE] = {0};
    int is_empty_flag = 0;
    int id = 1;
    printf(stderr,"     Pouch containers:\n");

    // Not including the console tty
    for(i=0; i < (MAX_TTY - 1); i++){
        tty[4] = '0' + i;
        if((tty_fd = open(tty, O_RDWR)) < 0){
            printf(stderr, "cannot open %s fd\n", tty);
            return -1;
        }
        if(read_from_pconf(tty,buf) <0 ){
            printf(stderr, "failed to read pconf from %s\n", tty);
            return -1;
        }
        if(!buf[0]){
            continue;
        }
        is_empty_flag = 1;
        printf(stderr,"%d. %s : ",id++, buf);
        empty_string(buf, sizeof(buf));

        if(is_connected_tty(tty_fd)){
            printf(stderr,"connected \n");
        }else{
            printf(stderr,"started \n");
        }
        close(tty_fd);
    }
    if(!is_empty_flag){
        printf(stderr,"None.\n");
    }

    return 0;
}

static int get_connected_cname(char * cname){
    int i;
    int tty_fd;
    char tty[] = "/ttyX";
    char buf[CNTNAMESIZE] = {0};

    // Not including the console tty
    for(i=0; i < (MAX_TTY - 1); i++){
        tty[4] = '0' + i;
        if((tty_fd = open(tty, O_RDWR)) < 0){
            printf(stderr, "cannot open %s fd\n", tty);
            return -1;
        }
        read_from_pconf(tty,buf);
        if(is_connected_tty(tty_fd)){
            strcpy(cname,buf);
        }
        close(tty_fd);
    }
    if(!buf[0]){
        return -1;
    }
    return 0;
}

static int read_from_cconf(char* container_name, char* tty_name, int* pid){
   char pid_str[sizeof("PPID:") + 10];
   int cont_fd = open(container_name, 0);

   if(cont_fd < 0){
      printf(stderr, "There is no container: %s in a started stage\n", container_name);
      exit(1);
    }

   if(read(cont_fd, tty_name, sizeof("/ttyX")) < sizeof("/ttyX")) {
      close(cont_fd);
      printf(stderr,"CONT TTY NOT FOUND\n");
      return -1;
   }

   tty_name[sizeof("/ttyX")-1] = 0;

   if(read(cont_fd, pid_str, sizeof(pid_str)) < sizeof("PPID:")+2) {
      close(cont_fd);
      printf(stderr,"CONT PID NOT FOUND\n");
      return -1;
   }

   pid_str[sizeof(pid_str)-1] = 0;
   *pid = atoi(pid_str+sizeof("PPID:"));

   close(cont_fd);
   return 0;
}

static int write_to_cconf(char* container_name, char* tty_name, int pid){
   int cont_fd = open(container_name, O_CREATE|O_RDWR);
   if(cont_fd < 0){
        printf(stderr, "cannot open %s\n", container_name);
        return -1;
   }
   printf(cont_fd, "%s\nPPID: %d\nNAME: %s\n",tty_name, pid, container_name);
   close(cont_fd);
   return 0;
}

static int pouch_fork(char* container_name){
   int tty_fd;
   int pid = -1;
   int pid2 = -1;
   char tty_name[10];
   char cg_cname[256];
   int daemonize = 1;

   //Find tty name
   if(find_tty(tty_name) < 0){
      printf(1, "Pouch: cannot create more containers\n");
      exit(1);
   }

   int cont_fd = open(container_name, 0);
   if(cont_fd < 0){
      printf(1, "Pouch: %s starting\n",container_name);
   }else{
      printf(stderr, "Pouch: %s container is already started\n", container_name);
      exit(1);
   }

   //Prepare cgroup name for container
   prepare_cgroup_cname(container_name,cg_cname);
   if(create_pouch_cgroup(cg_cname, container_name) <0)
      exit(1);

   //update cname in pouch conf
   write_to_pconf(tty_name,container_name);

   if((tty_fd = open(tty_name, O_RDWR)) < 0){
      printf(stderr, "cannot open tty %s %d\n", tty_name, tty_fd);
      return -1;
   }

   //Parent process forking child process
   if(!daemonize || (daemonize && (pid2 = fork()) == 0)){
      //Set up pid namespace before fork
      if(unshare(PID_NS) != 0){
        printf(stderr, "Cannot create pid namespace\n");
        return -1;
      }

      pid = fork();
      if(pid == -1){
         panic("fork");
      }

      if(pid == 0) {
         if(tty_fd != -1){
            //attach stderr stdin stdout
            if(attach_tty(tty_fd) < 0){
              printf(stderr,"attach failed");
              exit(1);
            }

           //"Child process - setting up namespaces for the container
           // Set up mount namespace.
           if(unshare(MOUNT_NS) < 0) {
             printf(1, "Cannot create mount namespace\n");
             exit(1);
           }

           printf(stderr,"Entering container\n");
           exec("sh", argv);
        }else{
           printf(stderr,"Error connecting tty\n");
        }
      }else{

        //"Parent process - waiting for child

        // Move the current process to "/cgroup/<cname>" cgroup.
        strcat(cg_cname,"/cgroup.procs");
        int cgroup_procs_fd = open(cg_cname, O_RDWR);
        char cur_pid_buf[10];
        itoa(cur_pid_buf, pid);
        if(write(cgroup_procs_fd, cur_pid_buf, sizeof(cur_pid_buf)) < 0)
            return -1;
        if(close(cgroup_procs_fd) < 0)
            return -1;
        if(write_to_cconf(container_name, tty_name, pid) >= 0)
           wait(0);



        exit(0);
      }
    }

  if(close(tty_fd) < 0)
      return -1;
  return 0;
}

static int print_help_inside_cnt(){
    int retval = 0;
    retval = printf(stderr,"\nPouch commands inside containers:\n");
    retval |= printf(stderr,"       pouch disconnect \n");
    retval |= printf(stderr,"          : disconnect a currently connected container\n");
    retval |= printf(stderr,"       pouch info\n");
    retval |= printf(stderr,"          : query information about currently connected container\n");
    return retval;
}

void print_help_outside_cnt(){
    printf(stderr,"\nPouch commands outside containers:\n");
    printf(stderr,"       pouch start {name}\n");
    printf(stderr,"          : starts a new container\n");
    printf(stderr,"          - {name} : container name\n");
    printf(stderr,"       pouch connect {name}\n");
    printf(stderr,"          : connect already started container\n");
    printf(stderr,"          - {name} : container name\n");
    printf(stderr,"       pouch destroy {name}\n");
    printf(stderr,"          : destroy a container\n");
    printf(stderr,"          - {name} : container name\n");
    printf(stderr,"       pouch info {name}\n");
    printf(stderr,"          : query information about a container\n");
    printf(stderr,"          - {name} : container name\n");
    printf(stderr,"       pouch list all\n");
    printf(stderr,"          : displays state of all created containers\n");
    printf(stderr,"      \ncontainers cgroups:\n");
    printf(stderr,"       pouch cgroup {cname} {state-object} [value]\n");
    printf(stderr,"          : limit given cgroup state-object\n");
    printf(stderr,"          - {name} : container name\n");
    printf(stderr,"          - {state-object} : cgroups state-object. Refer spec.\n");
    printf(stderr,"          - [value] : argument for the state-object, multiple values delimited by ','\n");
}

static int create_pouch_cgroup(char *cg_cname, char *cname){

    if(mkdir(cg_cname) != 0){
        printf(stderr,"Pouch: Failed to create cgroup with the given name. Consider another container name: %s \n",cname);
        return -1;
    }
    char cgpath[256];
    memset(cgpath,'\0',256);
    strcpy(cgpath, cg_cname);
    strcat(cgpath,"/cgroup.subtree_control");

    int cgroup_subtree_control_fd =
        open(cgpath, O_RDWR);

    if(cgroup_subtree_control_fd < 0)
        return -1;

    // Enable cpu controller
    char buf[256];
    memset(buf,'\0',256);
    strcpy(buf, "+cpu");
    if(write(cgroup_subtree_control_fd, buf, sizeof(buf)) < 0)
        return -1;
    if(close(cgroup_subtree_control_fd) < 0)
        return -1;
    return 0;

}

static int init_pouch_cgroup(){

    int cgroup_fd = -1;
    if((cgroup_fd = open("/cgroup", O_RDWR)) < 0){

        if(mkdir("/cgroup") != 0){
            printf(1, "Pouch: Failed to create root cgroup.\n");
            return -1;
        }
        if(mount(0, "/cgroup", "cgroup") != 0){
            printf(1, "Pouch: Failed to mount cgroup fs.\n");
            return -1;
        }
    }else{
        if(close(cgroup_fd) < 0)
            return -1;
    }

    return 0;
}

static int print_cinfo(char* container_name, char * tty_name, int pid){
    char buf[256];
    char cgmax[256];
    char cgstat[256];

    strcpy(cgmax, "/cgroup/");
    strcat(cgmax,container_name);
    strcat(cgmax,"/cpu.max");

    strcpy(cgstat, "/cgroup/");
    strcat(cgstat,container_name);
    strcat(cgstat,"/cpu.stat");

    int cpu_max_fd = open(cgmax, O_RDWR);
    int cpu_stat_fd = open(cgstat, O_RDWR);
    int ppid = getppid();

    if(ppid == 1){
        printf(1, "     Pouch container- %s: connected\n",container_name);
    }else{
        printf(1, "     Pouch container- %s: started\n",container_name);
    }

    printf(1,"tty - %s\n",tty_name);
    printf(1,"pid - %d\n",pid);
    printf(1,"     cgroups:\n");
    if(cpu_max_fd > 0 && cpu_stat_fd > 0){
        empty_string(buf, sizeof(buf));
        if(read(cpu_max_fd, buf, sizeof(buf)) < 0)
            return -1;
        printf(1, "cpu.max:     \n%s\n", buf);
        empty_string(buf, sizeof(buf));
        if(read(cpu_stat_fd, buf, sizeof(buf)) < 0)
            return -1;
        printf(1, "cpu.stat:     \n%s\n", buf);

        if(close(cpu_max_fd) < 0)
            return -1;
        if(close(cpu_stat_fd) < 0)
            return -1;
    }else{
        printf(1,"None.\n");
    }

    return 0;
}

int
main(int argc, char *argv[])
{
  enum p_cmd cmd = START;
  char container_name[CNTNAMESIZE];

  // get parent pid
  int ppid = getppid();

  if(argc >= 3){
     if((strcmp(argv[1],"--help") == 0) || (char)*argv[1] == '-'){
         if(ppid == 1)
            print_help_inside_cnt();
         else
             print_help_outside_cnt();
        exit(0);
     }
     strcpy(container_name, argv[2]);
  }else if(argc == 2){
      if(ppid == 1 && get_connected_cname(container_name) < 0){
          print_help_inside_cnt();
          exit(1);
      }
      else if(ppid != 1){
          print_help_outside_cnt();
          exit(0);
      }
  }else{
      if(ppid == 1)
          print_help_inside_cnt();
      else
          print_help_outside_cnt();
    exit(0);
  }

  // get command type
  if(argc >= 2){
     if((strcmp(argv[1],"start")) == 0){
        cmd = START;
     }else if((strcmp(argv[1],"connect")) == 0){
        cmd = CONNECT;
     }else if((strcmp(argv[1],"disconnect")) == 0){
        cmd = DISCONNECT;
        if(ppid != 1){
            printf(1, "Pouch: no container is connected\n");
            exit(1);
        }
     }else if((strcmp(argv[1],"destroy")) == 0){
        cmd = DESTROY;
     }else if((strcmp(argv[1],"cgroup")) == 0 && argc == 5){
        cmd = LIMIT;
     }else if((strcmp(argv[1],"info")) == 0 ){
        cmd = INFO;
     }else if((strcmp(argv[1],"list")) == 0 && (strcmp(argv[2],"all")) == 0 ){
        cmd = LIST;
     }else{
        if(ppid == 1)
            print_help_inside_cnt();
        else
            print_help_outside_cnt();
        exit(1);
     }

     if(init_pouch_cgroup() < 0){
         printf(1, "Pouch: cgroup operation failed.\n");
         exit(1);
     }

     if(init_pouch_conf() < 0){
         printf(1, "Pouch: operation failed.\n");
         exit(1);
     }

     // Inside the container the are only few commands permitted, disable others.
     if(ppid == 1 && cmd != LIMIT && cmd != DISCONNECT /* && cmd != LIST*/
             && cmd != INFO){
         if(cmd == START){
             printf(1, "Nesting containers is not supported.\n");
             exit(1);
         }
         else if(cmd == CONNECT){
             printf(1, "Nesting containers is not supported.\n");
             exit(1);
         }
         else if(cmd == DESTROY){
             printf(1, "Container can't be destroyed while connected.\n");
             exit(1);
         }else if(cmd == LIST){
             if (print_help_inside_cnt() < 0) {
                exit(1);
             }
         }
     }else{
         //command execution
         if(cmd == LIMIT && argc == 5){
             if(pouch_limit_cgroup(container_name, argv[3], argv[4]) < 0){
                 exit(1);
             }
         }else if(pouch_cmd(container_name, cmd) < 0 ){
             printf(1, "Pouch: operation failed.\n");
             exit(1);
         }
     }
  }

  exit(0);
}
