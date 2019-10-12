#include "types.h"
#include "user.h"
#include "stat.h"

char* my_strcat(char* destination, const char* source)
{
	// make ptr point to the end of destination string
	char* ptr = destination + strlen(destination);

	// Appends characters of source to the destination string
	while (*source != '\0')
		*ptr++ = *source++;

	// null terminate destination string
	*ptr = '\0';

	// destination is returned by standard strcat()
	return destination;
}

int
main(int argc, char *argv[])
{
  char buf[64];
  proclist(buf);
  printf(1, "%s\n", buf);
  exit();
}

