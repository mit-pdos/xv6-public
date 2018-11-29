#ifndef XV6_SIGNAL
#define XV6_SIGNAL

#define MUN_OF_SIGNALS 5
#define SIG_IGN -1
#define SIG_DFL -2
#define SIGINT 0
#define SIGCHLD 3

#define SIGFPE 0

//typeof void (* sighandler_t)(int);
//sighandler_t signal(int signum,sighandler_t handler);

#endif
