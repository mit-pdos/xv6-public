#include "types.h"
#include "stat.h"
#include "user.h"

#define bufferSizeBytes (512)
static char buf[bufferSizeBytes];

int
main(int argc, char *argv[])
{
  snprintf(buf, bufferSizeBytes, "Hello, World\n");
  printf(1, "%s", buf);
  snprintf(buf, bufferSizeBytes, "%d\n", argc);
  printf(1, "%s", buf);
  snprintf(buf, bufferSizeBytes, "%x\n", argc);
  printf(1, "%s", buf);
  snprintf(buf, 6, "Hello, World\n");
  printf(1, "%s", buf);
  snprintf(buf, 12, "\n0x%x\n", 0xcafebabe);
  printf(1, "%s", buf);
  snprintf(buf, 6, "\n0x%x\n", 0xcafebabe);
  printf(1, "%s", buf);
  snprintf(buf, 0, "\n0x%x\n", 0xdeadbeef);
  printf(1, "%s", buf);
  snprintf(buf, bufferSizeBytes, "\n%d\n", 0xdeadbeef);
  printf(1, "%s", buf);

  snprintf(buf, bufferSizeBytes, "\nprogram is <%s>\n", argv[0]);
  printf(1, "%s", buf);

  int index = 0;
  for(int i = 1; i < argc; i++) {
    index += snprintf(&buf[index], bufferSizeBytes - index, "<%s>\n", argv[i]);
  }
  printf(1, "%s", buf);

  for(int i = 1; i < argc; i++) {
    printf(1, "<%d>\n", i - 3);
  }
  for(int i = 1; i < argc; i++) {
    printf(1, "<%x>\n", i - 3);
  }
  
  exit();
}
