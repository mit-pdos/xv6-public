#include <types.h>
#include <user.h>
#include <stat.h>
#include <fcntl.h>

int main(void) {
    printf(1, "La cantidad de procesos en ejecucion en la CPU es %i\n", getprocs());
    exit();
}