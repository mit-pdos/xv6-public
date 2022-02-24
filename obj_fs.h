#ifndef XV6_OBJ_FS_H
#define XV6_OBJ_FS_H

#include "types.h"
#include "vfs_fs.h"
#include "stat.h"

// Other parameters:
#define MAX_OBJECT_NAME_LENGTH 20
#define SUPER_BLOCK_ID "\x01"
#define OBJECT_TABLE_ID "\x02"
#define ROOT_ID "\x03"
#define OBJ_ROOTINO 3

struct objsuperblock {
    uint storage_device_size;
    uint objects_table_offset;
    uint objects_table_size;
    // the last inode added
    uint last_inode;
    // variables to trace the file-system state
    uint bytes_occupied;
    uint occupied_objects;
    struct vfs_superblock vfs_sb;
};

/**
 * On-disk inode structure
 * Currently, all the inode data is located inside a single object. We can
 * improve the implementation by implementing an indirect structure as in the
 * original xv6 file system.
 *
 * Inodes object names are "inode" and then the object id as 4 bytes. Because
 * part of the bytes can be zero we must change them so the id won't be cut in
 * the middle. Hence, we set the highest bit on for all the bytes and don't
 * use it to store the id. Hence, each byte contains only 127 possible values
 * and not 255. To make up for the loss, we add another byte to the id.
 For example, "inode5" would be "inode\x85\x80\x80\x80\x80" for uint of size 4
 * bytes.
 *
 * Not like in the original xv6 implementation, here we have no limitation on
 * the amount of inodes and we don't preserve space for them. This is done by
 * the fact the inodes are regular objects.
 * Moreover the `dinode` doesn't contain the inode name becuse it is the name
 * of the object.
 */
struct obj_dinode {
    struct vfs_dinode vfs_dinode;
    // the object containning the data of this inode
    char data_object_name[MAX_OBJECT_NAME_LENGTH];
};


/**
 * 5 is `strlen("inode")`
 * sizeof(uint) + 1 is the size of the counter
 * 1 for null terminator
 */
#define INODE_NAME_LENGTH (5 + sizeof(uint) + 1 + 1)

#endif
