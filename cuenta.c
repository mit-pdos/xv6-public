#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void) {
printf(1 , "La cantidad de procesos en ejecucion en la CPU es %d\n", getprocs());
exit();
}
