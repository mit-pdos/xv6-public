#ifndef XV6_OBJ_FS_H
#define XV6_OBJ_FS_H

#include "types.h"

// Other parameters:
#define MAX_OBJECT_NAME_LENGTH 20
#define SUPER_BLOCK_ID "\x01"
#define OBJECT_TABLE_ID "\x02"
#define ROOT_ID "\x03"

struct objsuperblock {
    uint storage_device_size;
    uint objects_table_offset;
    uint objects_table_size;

    // variables to trace the file-system state
    uint bytes_occupied;
    uint occupied_objects;
};


#endif
