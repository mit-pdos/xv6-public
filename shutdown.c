#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
	write(1,"shutdown...\n",12);
	halt();
	return 0;
}
