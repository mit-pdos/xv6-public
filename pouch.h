/*
*   Pouch.h
*   - pouch container commands
*/
#ifndef POUCH_H
#define POUCH_H

typedef enum p_cmd {START, CONNECT, DISCONNECT, DESTROY, LIMIT, INFO, LIST} p_cmd;
#define CNTNAMESIZE 100
#define CNTARGSIZE 30
char *argv[] = { "sh", 0 };

/*
*   Pouch cmd:
*   - Pouch operation based on command type
*   @input: container_name,p_cmd
*   @output: none
*   @return: 0 - OK, != 0 - FAILURE
*/
static int pouch_cmd(char* container_name, enum p_cmd);


/*
*   Pouch fork:
*   - Starting new container and execute shell inside, waiting for container to exit
*   @input: container_name
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int pouch_fork(char* container_name);


/*
*   Finding a tty:
*   - Finds a free tty to be attached
*   @input: none
*   @output: tty_name
*   @return: 0 - OK, <0 - FAILURE
*/
static int find_tty(char* tty_name);


/*
*   Read from conf:
*   - Reading container information from container's object
*   @input: container_name
*   @output: tty_name, pid
*   @return: 0 - OK, <0 - FAILURE
*/
static int read_from_cconf(char* container_name, char* tty_name, int* pid);


/*
*   Write to conf:
*   - Writing container information to container's object
*   @input: container_name, tty_name, pid
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int write_to_cconf(char* container_name, char* tty_name, int pid);


/*
*   Init pouch cgroup:
*   - Creates root cgroup dir if not exists and mounts cgroups fs
*   @input: none
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int init_pouch_cgroup();


/*
*   Create pouch cgroup:
*   - Creates cgroup for a container inside root cgroup
*   - Enables cpu.controller
*   @input: cg_cname - cgroups fs path to a new cgroup
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int create_pouch_cgroup(char *cg_cname, char *cname);


/*
*   Limit pouch cgroup:
*   - Limits given state object for given container name and limit
*   @input: container_name, cgroup_state_obj, limitation
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int pouch_limit_cgroup(char* container_name, char* cgroup_state_obj, char* limitation);


/*
*   Prepate cgroup name:
*   - Create a path in cgroup fs for corresponding cname
*   @input: container_name
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int prepare_cgroup_cname(char* container_name, char* cgcname);


/*
*   Write to pconf
*   - pconf is a file that holds container name that is currently attached to a tty
*   - pconf name is similar to tty name, having knows naming structure to open it
*   - used for 'printing all containers list 'pouch list all' command
*   @input: container_name, cgcname
*   @return: 0 - OK, <0 - FAILURE
*/
static int write_to_pconf(char * ttyname, char * cname);


/*
*   Remove from pconf
*   - when container is deleted, tty is detatched, need to remove it's name from pconf
*   @input: ttyname
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int remove_from_pconf(char * ttyname);


/*
*   Read from pconf
*   - get container name from provided tty name
*   @input: ttyname
*   @output: cname
*   @return: 0 - OK, <0 - FAILURE
*/
static int read_from_pconf(char * ttyname, char * cname);


/*
*   Print cotainers list
*   - show all started containers and their state
*   @input: none
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int print_clist();


/*
*   Print given container information
*   - show all started containers and their state
*   @input: container_name,tty_name,pid
*   @output: none
*   @return: 0 - OK, <0 - FAILURE
*/
static int print_cinfo(char* container_name, char * tty_name, int pid);


/*
*   Get connected container name
*   @input: none
*   @output: cname
*   @return: 0 - OK, <0 - FAILURE
*/
static int get_connected_cname(char * cname);

#endif // POUCH_H

