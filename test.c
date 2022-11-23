#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
    char *virtual_address;
    virtual_address = argv[1];
    printf(1, "\n Dirección virtual ingresada: %s\n", virtual_address);
    getaddress(virtual_address);
    exit();
}