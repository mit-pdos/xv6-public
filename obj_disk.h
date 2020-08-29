#pragma once

/**
 * `obj_disk` specify the most lower level in the object file system.
 * This layer is responsible to communicate with the object storage device.
 *
 * In the implementation we assumed that the super-block is located in the
 * first bytes of the storage device. This object DOES located in the objects
 * table and has an id of 0. The objects table is an object as well.
 *
 * The hardware has a table which saves mapping between object id to its
 * address and its size. This limitaion is hardware dependent and should be
 * set when creating the file-system on specific drive. The size of the table
 * is saved in the super-block of the file system as well as the position of
 * it.
 *
 * In the current implementation, we use a "mock" storage device which saves
 * the data in the RAM. In future projcets, we can integrate with real 
 * hardware and only this file would change. It would become asynchrony and
 * handle hardware interrupts. The output interface this layer exports is
 * object based only. Hence, it doesn't matter for upper layers, if the
 * storage device handle the objects table itself or the kernel does it.
 * The inner implementation has the following structure:
 * The memory is an array os size `STORAGE_DEVICE_SIZE` which must be defined.
 * The super block is located at offset 0 and the table right after it. The
 * size of the table is set by the define `OBJECTS_TABLE_SIZE`.
 *
 * Currently, the objects table is implemented in a naive way of storing the
 * object's name as it's id. In the future, better implementation can be
 * implemented. One such implementation would use hash method on the name.
 * This hash method should be collision-free (sha256 would be sufficient
 * here). This would provide us the abillity to find the object without
 * full scanning the objects-table. In addition, this method would set all
 * the ids to be with the same length in bytes.
 * Because we currently doesn't use such method, we set an upper length for
 * the objects name. Hence, a relevant error can occour when caling
 *
 * In the current version, the disk is NOT thread safe.
 * TODO make it a thread safe.
 */

#include "types.h"


// Possible errors:
#define NO_ERR 0
#define OBJECT_EXISTS 1
#define OBJECT_NOT_EXISTS 2
#define OBJECT_NAME_TOO_LONG 3
#define OBJECTS_TABLE_FULL 4
#define NO_DISK_SPACE_FOUND 5


// Other parameters:
#define MAX_OBJECT_NAME_LENGTH 20
#define SUPER_BLOCK_ID ""
#define OBJECT_TABLE_ID "\x01"


// In the future, this can be set the size of SHA256 digest.
#define OBJECT_ID_LENGTH MAX_OBJECT_NAME_LENGTH


typedef struct {
    /*
     * If the object's name is exactly `MAX_OBJECT_NAME_LENGTH` we don't store
     * the null terminator.
     */
    char object_id[OBJECT_ID_LENGTH];
    uint disk_offset;
    uint size;
    int occupied;
} ObjectsTableEntry;


typedef struct {
    uint storage_device_size;
    uint objects_table_offset;
    uint objects_table_size;

    // variables to trace the file-system state
    uint bytes_occupied;
    uint occupied_objects;
} SuperBlock;


/**
 * Loads the super-block struct from the disk and set the state of the driver.
 *
 * In the current implementation, because we don't use real device, we
 * initialize the state of the super block manually to compilation known
 * values. In addition, we set the in-memory disk array.
 */
void init_obj_fs();

/**
 * Writes a new object of size `size` to the disk.
 * The name of the object is specified by the parameter `name` using a null
 * terminated string.
 * The method returns a code indicates the error occured.
 *   NO_ERR        - no error occured.
 *   OBJECT_EXISTS - object with this name already exists.
 *   OBJECT_NAME_TOO_LONG - the object exceedes `MAX_OBJECT_NAME_LENGTH`.
 *
 */
uint add_object(const void* object, uint size, const char* name);


/**
 * Rewrites the specified object to the disk.
 * If the new size is larger than the located one for the object, the method
 * search for a new address for the object. Before searching, the method
 * "deallocate" the object for the search. By that, the search method might
 * find the existing space large enough.
 *
 * The method returns a code indicates the error occured.
 *   NO_ERR            - no error occured.
 *   OBJECT_NOT_EXISTS - object with this name already exists.
 */
uint rewrite_object(const void* object, uint size, const char* name);


/**
 * Delete the specific object from the objects table. The bytes on the disk
 * does not change.
 * The method returns a code indicates the error occured.
 *   NO_ERR            - no error occured.
 *   OBJECT_NOT_EXISTS - no object with this name exists.
 */
uint delete_object(const char* name);


/**
 * Returns the size in bytes of the obejct with name `name`.
 * The method returns a code indicates the error occured.
 *   NO_ERR            - no error occured.
 *   OBJECT_NOT_EXISTS - no object with this name exists.
 */
uint object_size(const char* name, uint* output);


/**
 * Copy the desired object from the device storage to the output buffer.
 * The method assumes enough storage is given.
 * The method returns a code indicates the error occured.
 *   NO_ERR            - no error occured.
 *   OBJECT_NOT_EXISTS - no object with this name exists.
 */
uint get_object(const char* name, void* output);



/**
 * The following methods provides metadata about the file system state.
 * They can be used by programs that shows the state of the file-system such
 * as `df`.
 */


/**
 * The maximum possible objects in the file system. This amount includes the
 * file-system used objects such as inodes and the super block.
 * More specificaly, this value is the size of the objects table.
 */
uint max_objects(); 


/**
 * Specify how many objects are currently occupied in the table.
 */
uint occupied_objects(); 


/**
 * Returns the total size in bytes of the device. This value doesn't is not
 * related to the maximum objects supported by the device. Although it does
 * give it an upper limit.
 */
uint device_size(); 


/**
 * Returns the total size in bytes of the occupied objects including the
 * super-block and the objects table entry. Please notice that because of
 * fregmentation, the actual amount of data you can write might be lower.
 */
uint occupied_bytes();
