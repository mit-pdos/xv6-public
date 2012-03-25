// Tests to drive abstract sharing analysis

#include "types.h"
#include "user.h"
#include "mtrace.h"

int
main(int ac, char **av)
{
  int op;
  if (ac == 2 && strcmp(av[1], "on") == 0)
    op = 1;
  else if (ac == 2 && strcmp(av[1], "off") == 0)
    op = 0;
  else {
    fprintf(1, "usage: %s on|off\n", av[0]);
    exit();
  }

  if (op) {
    mtenable_type(mtrace_record_ascope, "xv6-asharing");
  } else {
    mtdisable("xv6-asharing");  
  }
}
