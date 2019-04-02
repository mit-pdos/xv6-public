#include "types.h"
#include "stat.h"
#include "defs.h"

int main(void) {
	cprintf("La cantidad de procesos en ejecucion en la CPU es %i\n", getprocs());
	return void;
}