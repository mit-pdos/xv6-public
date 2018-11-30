#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
	write(1,"Shutdown...\n",12);
	halt();
	exit();
}
