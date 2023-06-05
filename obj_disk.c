// Required for new gcc versions such as 9.4.0
#pragma GCC diagnostic ignored "-Wstack-usage="

#include "obj_disk.h"
#include "types.h"
#include "sleeplock.h"
#include "kvector.h"
#include "vfs_file.h"

#ifndef KERNEL_TESTS
#include "defs.h"  // import `panic`
#else
#include "obj_fs_tests_utilities.h"  // impot mock `panic`
#include "string.h"
#endif


#ifndef STORAGE_DEVICE_SIZE
#error "STORAGE_DEVICE_SIZE must be defined when using the mock storage device"
#endif

#define entry_index_to_entry_offset(index) super_block.objects_table_offset + index * sizeof(ObjectsTableEntry)

struct sleeplock disklock;

struct objsuperblock super_block;

char memory_storage[STORAGE_DEVICE_SIZE];

uint get_objects_table_index(const char* name, uint* output) {
    for (uint i = 0; i < get_object_table_size(); ++i) {
        ObjectsTableEntry* entry = objects_table_entry(i);
        if (entry->occupied &&
            obj_id_cmp(entry->object_id, name) == 0
        ) {
            *output = i;
            return NO_ERR;
        }
    }

    return OBJECT_NOT_EXISTS;
}


ObjectsTableEntry* objects_table_entry(uint entry_index) {
    return (ObjectsTableEntry*)&memory_storage[entry_index_to_entry_offset(entry_index)];
}


uint flush_objects_table_entry(uint offset) {
    /**
     * Currently, because the implemntation is memory only, writing to the
     * entry received by `objects_table_entry` changes the table itself.
     * In the future, this method would write the specific bytes to the disk.
     */

    return NO_ERR;
}


int obj_id_cmp(const char* p, const char* q) {
    uint i = 0;
    while(*p && *p == *q && i < OBJECT_ID_LENGTH) {
        p++;
        q++;
        i++;
    }
    return (char)*p - (char)*q;
}


uint obj_id_bytes(const char* object_id) {
    uint bytes = strlen(object_id);
    if (bytes < MAX_OBJECT_NAME_LENGTH) {
        bytes++;  // null terminator as well.
    }
    return bytes;
}


void swap(uint* xp, uint* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}


void bubble_sort(uint* arr, uint n) {
    if (n >= 2){
        for (uint i = 0; i < n - 1; i++) {
            for (uint j = 0; j < n - i - 1; j++) {
                if (objects_table_entry(arr[j])->disk_offset > objects_table_entry(arr[j + 1])->disk_offset) {
                    swap(&arr[j], &arr[j + 1]);
                }
            }
        }
    }
}

/**
 * The method finds a sequence of empty bytes of length `size`.
 * If no such sequence exists, NULL is returned.
 *
 * To achieve this, the method first sort the objects in the table by their
 * address in the disk. Then, we check the space between every 2 consecutive
 * entries. We also check the space left between the end of the last element
 * and the end of the disk.
 * Notice that there are at least 2 obejcts in the table at every moment: the
 * super block and the objects table itself. Also, the super block is located
 * at address 0, hence we don't check the "empty space" before it because
 * there is no such space.
 *
 * Smarter implementation can be used in future work such as saving free
 * blocks in a list. Read "malloc internals" for details.
 */
static void* find_empty_space(uint size) {
    // 1. put all occupied entries in an array
    uint current_table_size = min((super_block.store_offset - super_block.objects_table_offset),STORAGE_DEVICE_SIZE) / sizeof(ObjectsTableEntry);
    const uint entries_arr_size = current_table_size - 2;
    uint entries_indices[entries_arr_size];
    uint* current = entries_indices;
    uint populated_size = 0;
    for (uint i = 2; i < get_object_table_size(); ++i) {
        if (objects_table_entry(i)->occupied) {
            *current = i;
            current++;
            populated_size++;
            if (current - entries_indices > entries_arr_size) {
                panic("found too much entries vs expected");
            }
        }
    }

    if(populated_size == 0 && STORAGE_DEVICE_SIZE-size >= super_block.store_offset){
        return &memory_storage[STORAGE_DEVICE_SIZE-size];
    }

    uint last_occupied_entry = entries_indices[entries_arr_size-1];
    // 2. sort it by objects disk-offset. smaller first.
    bubble_sort(entries_indices, populated_size);
    // 3. Check for rightmost spot 
    ObjectsTableEntry* last_entry = objects_table_entry(
        entries_indices[populated_size-1]
    );
    uint space_left = device_size()
                      - last_entry->disk_offset
                      - last_entry->size;
    if (space_left >= size) {
        return &memory_storage[STORAGE_DEVICE_SIZE-size];
    }
    // 4. for each occupied entry, measure space between this entry to the next.
    // if an appropriate space is found, return the rightmost memory chunk for allocation.
    if (populated_size >= 2){
        for (uint i = populated_size - 1; i > 0; i--) {
            ObjectsTableEntry* current_entry = objects_table_entry(entries_indices[i]);
            ObjectsTableEntry* prev_entry = objects_table_entry(entries_indices[i - 1]);
            space_left = current_entry->disk_offset
                              - prev_entry->disk_offset
                              - prev_entry->size;
            if (space_left >= size) {
                return &memory_storage[current_entry->disk_offset - size];
            }
        }
    }
    // 5. could not find any space, consider moving the store space limit.
    ObjectsTableEntry* earliest_occupied_entry = objects_table_entry(entries_indices[0]);
    if (earliest_occupied_entry->disk_offset - super_block.store_offset >= size){
        return &memory_storage[earliest_occupied_entry->disk_offset - size];
    }else{
        uint needed_space = size - (earliest_occupied_entry->disk_offset - super_block.store_offset);
        uint offset_after_last_entry = entry_index_to_entry_offset(last_occupied_entry+1);
        if(offset_after_last_entry < super_block.store_offset - needed_space){
            set_store_offset(super_block.store_offset - needed_space);
            return &memory_storage[super_block.store_offset];
        }
    }
    // 6. no solution without defragmentation.
    return NULL;
}


static void initialize_super_block_entry() {
    ObjectsTableEntry* entry = objects_table_entry(0);
    memmove(entry->object_id, SUPER_BLOCK_ID, strlen(SUPER_BLOCK_ID) + 1);
    entry->disk_offset = 0;
    entry->size = sizeof(super_block);
    entry->occupied = 1;
}


static void initialize_objects_table_entry() {
    ObjectsTableEntry* entry = objects_table_entry(1);
    memmove(entry->object_id, OBJECT_TABLE_ID, strlen(OBJECT_TABLE_ID) + 1);
    entry->disk_offset = super_block.objects_table_offset;
    entry->size = INITIAL_OBJECT_TABLE_SIZE * sizeof(ObjectsTableEntry);
    entry->occupied = 1;
}


// the disk lock should be held by the caller
static void write_super_block() {
    memmove(memory_storage, &super_block, sizeof(super_block));
}


void init_obj_fs() {
    struct vfs_superblock sb;
    // with real device, we would read the block form the disk.
    initsleeplock(&disklock, "disklock");

    // Super block initializing
    super_block.storage_device_size = STORAGE_DEVICE_SIZE;
    super_block.objects_table_offset = sizeof(struct objsuperblock);
    super_block.store_offset = super_block.objects_table_offset + 200 * sizeof(ObjectsTableEntry); //initial state only
    super_block.bytes_occupied =
        sizeof(super_block)
        + INITIAL_OBJECT_TABLE_SIZE * sizeof(ObjectsTableEntry);
    super_block.occupied_objects = 2;
    super_block.last_inode = 2; // Inode counter starts from 3, when 3 reserved to root dir object.
    sb.ninodes = get_object_table_size(); //TODO: remove it? it is now meaningless since inode number can grow and shrink. analyze effect over vfs.
    super_block.vfs_sb = sb;
    // Inode initializing

    // To keep consistency, we write the super block to the disk and sets the
    // table state. This part should be read from the device and be created
    // when initializing the disk.
    for (uint i = 0; i < get_object_table_size(); ++i) {
        objects_table_entry(i)->occupied = 0;
    }
    write_super_block();
    initialize_super_block_entry();
    initialize_objects_table_entry();
}


uint find_space_and_populate_entry(ObjectsTableEntry * entry, const void* object, const char* name, uint size) {
    void* address = find_empty_space(size);
    if (!address) {
        releasesleep(&disklock);
        return NO_DISK_SPACE_FOUND;
    }
    memmove(entry->object_id, name, obj_id_bytes(name));
    entry->disk_offset = address - (void*)memory_storage;
    entry->size = size;
    memmove(address, object, size);
    entry->occupied = 1;
    super_block.bytes_occupied += size;
    super_block.occupied_objects += 1;
    write_super_block();
    releasesleep(&disklock);
    return NO_ERR;
}


uint add_object(const void* object, uint size, const char* name) {
    // 1. check if the object is already present in disk
    uint err = check_add_object_validality(size, name);
    if (err != NO_ERR) {
        return err;
    }
    // 2. find first unocuupied entry of the objects table
    // then occupy it and allocate space for the new object.
    acquiresleep(&disklock);
    uint leftmost_disk_allocation_offset = STORAGE_DEVICE_SIZE; 
    uint i;
    for (i = 0; i < get_object_table_size(); i++) {
        ObjectsTableEntry* entry = objects_table_entry(i);
        if(entry->disk_offset < leftmost_disk_allocation_offset)
            leftmost_disk_allocation_offset = entry->disk_offset;
        if (!entry->occupied) {
            return find_space_and_populate_entry(entry, object, name,  size);
        }
    }
    // 3. all entries are occupied. is it possible to extend the table?
    // find offset of the first object.
    if(leftmost_disk_allocation_offset - super_block.store_offset  >= sizeof(ObjectsTableEntry)){
        set_store_offset(super_block.store_offset+sizeof(ObjectsTableEntry));
        ObjectsTableEntry* entry = objects_table_entry(i);
        return find_space_and_populate_entry(entry, object, name,  size);
    }
    releasesleep(&disklock);
    return OBJECTS_TABLE_FULL;
}


uint rewrite_object(vector data, uint objectsize, uint write_starting_offset, const char* name) {
    uint err;
    // 1. check for name contraints validity
    err = check_rewrite_object_validality(objectsize, name);
    if (err != NO_ERR) {
        return err;
    }
    // 2. name is ok. get the index off the object's entry
    acquiresleep(&disklock);
    uint i;
    err = get_objects_table_index(name, &i);
    if (err != NO_ERR) {
        releasesleep(&disklock);
        return err;
    }
    ObjectsTableEntry* entry = objects_table_entry(i);
    super_block.bytes_occupied -= entry->size;
    if (entry->size >= objectsize) {
        // 3.A - the new object written is smaller or equals the the original.
        void* address =
            (void*)memory_storage + entry->disk_offset + write_starting_offset;
        memmove_from_vector(address, data, 0, data.vectorsize); //TODO? instead of data.vectorsize add parameter 'datasize'
        entry->size = objectsize;
    } else {
        //3.B - the new object is larger
        entry->occupied = 0;
        super_block.occupied_objects -= 1;
        void* new_object_address = find_empty_space(objectsize);
        void* data_destination_address = new_object_address + write_starting_offset;
        entry->occupied = 1;
        super_block.occupied_objects += 1;
        if (!new_object_address) {
            releasesleep(&disklock);
            return NO_DISK_SPACE_FOUND;
        }
        memmove(new_object_address, (void*)memory_storage + entry->disk_offset, entry->size);
        memmove_from_vector(data_destination_address, data, 0, data.vectorsize);
        entry->size = objectsize;
        entry->disk_offset = new_object_address - (void*)memory_storage;
    }
    super_block.bytes_occupied += objectsize;
    write_super_block();
    releasesleep(&disklock);
    return NO_ERR;
}


uint object_size(const char* name, uint* output) {
    if (strlen(name) > MAX_OBJECT_NAME_LENGTH) {
        return OBJECT_NAME_TOO_LONG;
    }
    acquiresleep(&disklock);
    uint i;
    uint err = get_objects_table_index(name, &i);
    if (err != NO_ERR) {
        releasesleep(&disklock);
        return err;
    }
    ObjectsTableEntry* entry = objects_table_entry(i);
    *output = entry->size;
    releasesleep(&disklock);
    return NO_ERR;
}


uint get_object(const char* name, void* output, vector * outputvector) {
    // 1. make sure the name is of legal length
    if (strlen(name) > MAX_OBJECT_NAME_LENGTH) {
        return OBJECT_NAME_TOO_LONG;
    }
    // 2. try to locate the object in the object-table
    // return an index i or an error code
    acquiresleep(&disklock);
    uint i;
    uint err = get_objects_table_index(name, &i);
    if (err != NO_ERR) {
        releasesleep(&disklock);
        return err;
    }
    // 3. read the objects offset in disk, then read the object into
    // output address and vector
    ObjectsTableEntry* entry = objects_table_entry(i);
    void* address = (void*)memory_storage + entry->disk_offset;
    if(output != NULL)
        memmove(output, address, entry->size);
    if(outputvector != NULL)
        memmove_into_vector_bytes(*outputvector, 0, address, entry->size);
    releasesleep(&disklock);
    return NO_ERR;
}


uint delete_object(const char* name) {
    uint err = check_delete_object_validality(name);
    if (err != NO_ERR) {
        return err;
    }
    acquiresleep(&disklock);
    uint i;
    err = get_objects_table_index(name, &i);
    if (err != NO_ERR) {
        releasesleep(&disklock);
        return err;
    }
    ObjectsTableEntry* entry = objects_table_entry(i);
    entry->occupied = 0;
    super_block.occupied_objects -= 1;
    super_block.bytes_occupied -= entry->size;
    write_super_block();
    releasesleep(&disklock);
    return NO_ERR;
}


uint check_add_object_validality(uint size, const char* name) {
    // currently, because we don't use hash function, we must first scan the
    // table and check if the object already exists.
    if (strlen(name) > MAX_OBJECT_NAME_LENGTH) {
        return OBJECT_NAME_TOO_LONG;
    }
    for (uint i = 0; i < get_object_table_size(); ++i) {
        if (objects_table_entry(i)->occupied &&
            obj_id_cmp(objects_table_entry(i)->object_id, name) == 0) {
            return OBJECT_EXISTS;
        }
    }
    return NO_ERR;
}


uint check_rewrite_object_validality(uint size, const char* name) {
    if (strlen(name) > MAX_OBJECT_NAME_LENGTH) {
        return OBJECT_NAME_TOO_LONG;
    }
    return NO_ERR;
}


uint check_delete_object_validality(const char* name) {
    if (strlen(name) > MAX_OBJECT_NAME_LENGTH) {
        return OBJECT_NAME_TOO_LONG;
    }
    return NO_ERR;
}


uint new_inode_number() {
    acquiresleep(&disklock);
    super_block.last_inode++;
    write_super_block();
    releasesleep(&disklock);
    return super_block.last_inode;
}


uint get_object_table_size() {
    return (super_block.store_offset - super_block.objects_table_offset) / sizeof(ObjectsTableEntry);
}


uint occupied_objects() {
    return super_block.occupied_objects;
}


void set_occupied_objects(uint value) {
    acquiresleep(&disklock);
    super_block.occupied_objects = value;
    write_super_block();
    releasesleep(&disklock);
}


void set_store_offset(uint new_offset) {
    int acquired = 0;
    if(!holdingsleep(&disklock)){
        acquired = 1;
        acquiresleep(&disklock);
    }
    super_block.store_offset = new_offset;
    write_super_block();
    if(acquired){
        releasesleep(&disklock);
    }
}


uint device_size() {
    return super_block.storage_device_size;
}


uint occupied_bytes() {
    return super_block.bytes_occupied;
}
