#ifndef XV6_OBJ_DISK_H
#define XV6_OBJ_DISK_H

#include "obj_fs.h"
#include "types.h"
#include "kvector.h"

/**
 * `obj_disk` specify the most lower level in the object file system.
 * This layer is responsible to communicate with the object storage device.
 *
 * Object file systems use a table containning the objects meta data. The
 * table has the following structure:
 * |
 *
 * In our implementation we assume the table is software implemented. Some
 * hardware devices can implement it in the hardware level to improve
 * performance and relaiability. By implementing the table outselfs, the only
 * requierment we have from the hardware is being abale to request any bytes
 * we want in random access method. Especially without any aligment. Still,
 * hardwares have maximum transaction size. For example, we can only write X
 * bytes in a single request. This X is usually 4KB but the implementation
 * would be generic.
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
 * Hardware Mock
 * =============
 * In the current implementation, we use a "mock" storage device which saves
 * the data in the RAM. In future projcets, we can integrate with real 
 * hardware and only this file would change. It would become asynchrony and
 * handle hardware interrupts. The output interface this layer exports is
 * object based only. Hence, it doesn't matter for upper layers, if the
 * storage device handle the objects table itself or the kernel does it.
 * When changing to interrupt-implemented driver, we could extend upper levels
 * in the file system as well. For example, the logbook layer - more details
 * on that in the relevant files.
 *
 * As we mentioned before, each transaction has a maximum size. Hence, when
 * writing large objects to the disk (or reading them) we need to split them
 * into smaller requests. This implementation depends on the interrupt
 * handling flow that communicates with the hardware. Hence, we didn't add
 * such logic to the mock implementation. We do the transaction from start to
 * end in a single operation.
 *
 * The inner implementation has the following structure:
 * The memory is an array of size `STORAGE_DEVICE_SIZE` which must be defined.
 * The super block is located at offset 0 and the table right after it.
 *
 * Futher improvments
 * ==================
 * Currently, the objects table is implemented in a naive way of storing the
 * object's name as it's id. In the future, better implementation can be
 * implemented. One such implementation would use hash method on the name.
 * This hash method should be collision-free (sha256 would be sufficient
 * here). This would provide us the abillity to find the object without
 * full scanning the objects-table. In addition, this method would set all
 * the ids to be with the same length in bytes.
 * Because we currently doesn't use such method, we set an upper length for
 * the objects name. Hence, a relevant error can occour when calling
 *
 * In the current version, the disk is NOT thread safe.
 * TODO make it a thread safe.
 */

// The object table is of variable size.
// However, this is its default initial size.
// During the creation and deletion of files
// this table can grow in otder to index many files,
// Or shrink, making room for content storage.
#define INITIAL_OBJECT_TABLE_SIZE 200

// Possible errors:
#define NO_ERR 0
#define OBJECT_EXISTS 1
#define OBJECT_NOT_EXISTS 2
#define OBJECT_NAME_TOO_LONG 3
#define OBJECTS_TABLE_FULL 4
#define NO_DISK_SPACE_FOUND 5


// In the future, this can be set the size of SHA256 digest.
#define OBJECT_ID_LENGTH MAX_OBJECT_NAME_LENGTH

extern char memory_storage[STORAGE_DEVICE_SIZE];

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

int obj_id_cmp(const char* p, const char* q);
uint obj_id_bytes(const char* object_id);

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
 * This functions receives an object of size 'objectsize'
 * and rewrites some of its content which is specified
 * by the offset parameter.
 * This function will then write the data contained in the vector
 * to the file, starting the given offset.
 * If the new size is larger than the located one for the object, the method
 * search for a new address for the object. Before searching, the method
 * "deallocate" the object for the search. By that, the search method might
 * find the existing space large enough.
 *
 * The method returns a code indicates the error occured.
 *   NO_ERR            - no error occured.
 *   OBJECT_NOT_EXISTS - object with this name already exists.
 */
uint rewrite_object(vector object, uint objectsize, uint write_starting_offset, const char* name);


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
uint get_object(const char* name, void* output, vector * outputvector);



/**
 * The following methods are utility methods to help restore the disk in case
 * of state failures. The usages are fixing a corrupted disk by utility
 * applications. The methods are accessing the disk in RAW and without safety
 * guards. This methods should only be used by functions in this file and
 * disk-restore applications.
 */

/**
 * Returns the object index in the objects table.
 *   NO_ERR            - no error occured.
 *   OBJECT_NOT_EXISTS - no object with this name exists.
 */
uint get_objects_table_index(const char* name, uint* output);


/**
 * Returns the specific entry from the objects table.
 */
ObjectsTableEntry* objects_table_entry(uint offset);

/**
 * Writes a specfic objects table entry to the disk.
 */
uint flush_objects_table_entry(uint offset);



/**
 * Validation methods to ensure safe operations. They are automaticaly used by
 * the main operations. You can use them when you want to do some logic before
 * adding/changing the objects and want to make sure the operation would
 * success. In such cases, it's the funcion user responsability to make sure
 * the state stays the same until the operation is done.
 */
//@{
uint check_add_object_validality(uint size, const char* name);
uint check_rewrite_object_validality(uint size, const char* name);
uint check_delete_object_validality(const char* name);
//@}

uint new_inode_number();

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
uint get_object_table_size();

/**
 * Specify how many objects are currently occupied in the table.
 */
uint occupied_objects(); 

/**
 * Setter function to update the occupied objects counter in the super block.
 * Note: This value should be changed only by the functions in this file and
 * applications which fix failures in the disk.
 */
void set_occupied_objects(uint value); 


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


/**
 * Resize the object table and the store itself
 * by setting the limit between them to a specified value.
 */
void set_store_offset(uint new_offset);

#endif /* XV6_OBJ_DISK_H */

