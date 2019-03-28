#include "types.h"
#include "stat.h"


int main(void) {
	printf(1,"La cantidad de procesos en ejecucion en la CPU es %i\n", getprocs());
	exit();
}