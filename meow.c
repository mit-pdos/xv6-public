#include "user.h"

void test_lseek(){
  char buf[128];
  int fd = open("zombie", O_RDONLY);

  lseek(fd, 20, SEEK_SET);
  int bytesRead = read(fd, buf, 128);
  printf(1, "%d %d\n", bytesRead, buf[32]);

  bytesRead = read(fd, buf, 128);
  printf(1, "%d %d\n", bytesRead, buf[32]);

  lseek(fd, 20, SEEK_SET);
  bytesRead = read(fd, buf, 128);
  printf(1, "%d %d\n", bytesRead, buf[32]);

}

struct iovec {
    void* iov_base; /* Starting address */
    int iov_len; /* Length in bytes */
};

void test_writev(){
  static char part1[] = "THIS IS FROM WRITEV";
  static char part2[] = "[";
  static int  part3 = 65;
  
  struct iovec iov[3];

  iov[0].iov_base = part1;
  iov[0].iov_len = strlen(part1);

  iov[1].iov_base = part2;
  iov[1].iov_len = strlen(part2);

  iov[2].iov_base = &part3;
  iov[2].iov_len = sizeof(int);

  printf(1, "iov[0]: ");
  printf(1, "%s\n", part1);
  printf(1, "iov[1]: ");
  printf(1, "%s\n", part2);
  printf(1, "iov[2]: ");
  printf(1, "%c\n", part3);
  printf(1, "\n");

  printf(1, "writev(1, iov, 3): \n");
  writev(1, iov, 3);

}
int main(int argc, char *argv[])
{
  printf(1, "lseek(): \n");
  test_lseek();
  printf(1, "\n");

  printf(1, "writev(): \n");
  test_writev();

  printf(1, "\n\n");
  exit();
}
