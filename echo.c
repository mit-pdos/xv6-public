#include "types.h"
#include "stat.h"
#include "user.h"

/**
 * Usage: echo [-n] [text...]
 * Prints the input text to the stdout using the write system call.
 * The command will insert a new line at the end of the text except if the -n switch
 * is provided.
 * Notes: It is important for text to be written in one write call to allow
 * echo to interact with special kernel files such as for cgroups.
 */

int
main(int argc, char *argv[])
{
  int i = 1;
  int j = 0;
  int size = 0;
  char * data = 0;
  int offset = 0;
  char new_line = 1;
  int result = 0;

  if (argc > 1 && !strcmp("-n", argv[1])) {
    ++i;
    new_line = 0;
  }

  for (j = i; j < argc; ++j) {
    size += strlen(argv[j]) + 1;
  }

  data = malloc(size);

  if (!data) {
    exit(1);
  }

  for (; i < argc; ++i) {
    int length = strlen(argv[i]);
    memmove(data + offset, argv[i], length);
    offset += length;
    if (i + 1 < argc) {
      data[offset] = ' ';
      ++offset;
    }
  }

  if (new_line) {
    data[offset] = '\n';
    ++offset;
  }

  result = write(1, data, offset);
  free(data);

  if (-1 == result) {
    exit(1);
  }

  exit(0);
}
