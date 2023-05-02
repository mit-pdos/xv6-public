#include "types.h"
#include "stat.h"
#include "user.h"

//change this to change how many hex/ascii per line
#define CHAR_COUNT 16
#define LC_PER_CONTINUE 16

static void
ctoahex(char val, char buf[2])
{
	char l = (val & 0xF);
	buf[1] = (l <= 9 ? ('0' + l) : ('A' + l - 10));
	char h = (val & 0xF0) >> 4;
	buf[0] = (h <= 9 ? ('0' + h) : ('A' + h - 10));
}

static void
print_swath(const char* swath, int count)
{
	char hbuf[3] = { 'X', 'X', ' ' }; //index 2 never modified
	//hex values
	for (int i = 0; i < count; i++)
	{
		ctoahex(swath[i], hbuf);
		if (write(1, hbuf, 3) != 3)
		{
			printf(1, "hexdump: write error 1\n");
			exit();
		}
		if (i % 4 == 3) //print extra gap every 4
		{
			hbuf[0] = ' ';
			hbuf[1] = ' ';
			if (write(1, hbuf, 3) != 3)
			{
				printf(1, "hexdump: write error 2\n");
				exit();
			}
		}
	}
	//fill in empty hex spaces
	hbuf[0] = ' ';
	hbuf[1] = ' ';
	//spacing + missed "extra gap every 4" (little gap between hex and ascii)
	for (int i = 0; i < (CHAR_COUNT - count) + (4 - (count / 4)); i++)
	{
		if (write(1, hbuf, 3) != 3)
		{
			printf(1, "hexdump: write error 3\n");
			exit();
		}
	}
	//ascii values
	for (int i = 0; i < count; i++)
	{
		char bf[1] = { swath[i] };
		if ((bf[0] < '!' || bf[0] > '~') && bf[0] != ' ')
			bf[0] = '.';
		if (write(1, bf, 1) != 1)
		{
			printf(1, "hexdump: write error 4\n");
			exit();
		}
	}
	char nl[1] = { '\n' };
	if (write(1, nl, 1) != 1)
	{
		printf(1, "hexdump: write error 5\n");
		exit();
	}

//bad:
//	printf(1, "hexdump: write error\n");
//	exit();
}

char buf[512];

void
hexdump(int fd)
{
	int n;

	while((n = read(fd, buf, sizeof(buf))) > 0) {

		const char rem = n % CHAR_COUNT;
		for (int chunk = 0, lc = 0; chunk < n - rem; chunk += CHAR_COUNT, lc++)
		{
			char* section = buf + chunk;
			print_swath(section, CHAR_COUNT);
			if (lc >= LC_PER_CONTINUE)
			{
				printf(1, "Enter to continue, or type anything followed by enter to quit.\n");
				char ri[1] ={ '\n' };
				if (read(0, ri, sizeof(ri)) < 0 || ri[0] != '\n')
				{
					//consume all of standard input (so xv6 doesn't try to exec it).
					//technechally, if the user does a multiple of 512 chars as input, 
					//you need to type something & hit enter a second time.
					//This is done this way because there doesn't seem to be a way to
					//clear the stdin and/or a pipe. Perhaps add a syscall? Idk.
					while ((n = read(0, buf, sizeof(buf))) == sizeof(buf))
						;
					exit();
				}
				lc = 0;
			}
		}
		if (rem > 0)
			print_swath(buf + (n - rem), rem);
	}
	if (n < 0) {
		printf(1, "hexdump: read error\n");
		exit();
	}
}

int
main(int argc, char* argv[])
{
	int fd, i;

	if (argc <= 1) {
		printf(1, "hexdump: need file argument\n");
		exit();
	}

	for (i = 1; i < argc; i++) {
		if ((fd = open(argv[i], 0)) < 0) {
			printf(1, "hexdump: cannot open %s\n", argv[i]);
			exit();
		}
		hexdump(fd);
		close(fd);
	}
	exit();
}
