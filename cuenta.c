#include "types.h"
#include "stat.h"
#include "defs.h"
#include "stdio.h"



int main(void) {
	printf("La cantidad de procesos en ejecucion en la CPU es %i\n", getprocs());
	exit();
}