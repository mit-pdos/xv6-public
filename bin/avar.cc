// Tests to drive abstract sharing analysis

#include "types.h"
#include "user.h"
#include "mtrace.h"

int
main(int ac, char **av)
{
  if (ac == 2 && strcmp(av[1], "on") == 0)
    mtenable_type(mtrace_record_ascope, "xv6-asharing");
  else if (ac == 2 && strcmp(av[1], "off") == 0)
    mtdisable("xv6-asharing");  
  else
    fprintf(1, "usage: %s on|off\n", av[0]);
}
