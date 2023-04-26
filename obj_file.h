#ifndef XV6_OBJ_FILE_H
#define XV6_OBJ_FILE_H

#include "obj_fs.h"
#include "sleeplock.h"
#include "cgfs.h"
#include "param.h"
#include "vfs_file.h"

#define MAX_INODE_OBJECT_DATA (1ull << 25)  // 32MB

// in-memory copy of an inode
// that also keeps the mapping between an object in our object store and it's vfs_inode counterpart
struct obj_inode {
  char data_object_name[MAX_OBJECT_NAME_LENGTH];
  struct vfs_inode vfs_inode;
};

#define CONSOLE 1

#endif /* XV6_OBJ_FILE_H */

