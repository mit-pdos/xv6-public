#include "types.h"
#include "stat.h"
#include "user.h"
#include "proc_info.h"

int main(void)
{
  //struct proc_info *p = malloc(100 * sizeof(struct proc_info));
  //struct proc_info **p_infos = &p;
  
  struct proc_info *p;

  rnps(&p);
  exit();
}
