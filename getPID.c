#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char* argv[]) 
{
printf(1, "trying to execute\n");

printf(1, "Process info goes here %d\n", getpinfo());
printf(1, "This is where we set tickets %d\n", settickets(atoi(argv[1])));

exit();
} 