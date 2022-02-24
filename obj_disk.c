////#include <stdlib.h>
////#include <string.h>

#include "obj_disk.h"
#include "types.h"
#include "sleeplock.h"

#ifndef KERNEL_TESTS
#include "defs.h"  // import `panic`
#else
#include "obj_fs_tests_utilities.h"  // impot mock `panic`
#include "string.h"
#endif


#ifndef STORAGE_DEVICE_SIZE
#error "STORAGE_DEVICE_SIZE must be defined when using the mock storage device"
#endif

#ifndef OBJECTS_TABLE_SIZE
#error "OBJECTS_TABLE_SIZE must be defined when using the mock storage device"
#endif


struct sleeplock disklock;

struct objsuperblock super_block;

uint get_objects_table_index(const char* name, uint* output) {
    for (uint i = 0; i < max_objects(); ++i) {
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


ObjectsTableEntry* objects_table_entry(uint offset) {
    return (ObjectsTableEntry*)&memory_storage[
        super_block.objects_table_offset
        + offset * sizeof(ObjectsTableEntry)
    ];
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
    for (uint i = 0; i < n - 1; i++) {
        for (uint j = 0; j < n - i - 1; j++) {
            if (objects_table_entry(arr[j])->disk_offset > objects_table_entry(arr[j + 1])->disk_offset) {
                swap(&arr[j], &arr[j + 1]);
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
    uint entries_indices[OBJECTS_TABLE_SIZE];
    uint* current = entries_indices;
    for (uint i = 0; i < max_objects(); ++i) {
        if (objects_table_entry(i)->occupied) {
            *current = i;
            ++current;
            if (current - entries_indices > super_block.occupied_objects) {
                panic("found too much entries vs expected");
            }
        }
    }

    bubble_sort(entries_indices, super_block.occupied_objects);


    for (uint i = 0; i < super_block.occupied_objects - 1; ++i) {
        ObjectsTableEntry* current_entry = objects_table_entry(entries_indices[i]);
        ObjectsTableEntry* next_entry = objects_table_entry(entries_indices[i + 1]);
        uint space_left = next_entry->disk_offset
                          - current_entry->disk_offset
                          - current_entry->size;
        if (space_left >= size) {
            return &memory_storage[current_entry->disk_offset
                                   + current_entry->size];
        }
    }
    ObjectsTableEntry* last_entry = objects_table_entry(
        entries_indices[super_block.occupied_objects - 1]
    );
    uint space_left = device_size()
                      - last_entry->disk_offset
                      - last_entry->size;
    if (space_left >= size) {
        return &memory_storage[last_entry->disk_offset
                               + last_entry->size];
    }
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
    entry->size = OBJECTS_TABLE_SIZE * sizeof(ObjectsTableEntry);
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
    super_block.objects_table_size = OBJECTS_TABLE_SIZE;
    super_block.bytes_occupied =
        sizeof(super_block)
        + OBJECTS_TABLE_SIZE * sizeof(ObjectsTableEntry);
    super_block.occupied_objects = 2;
    super_block.last_inode = 2; // Inode counter starts from 3, when 3 reserved to root dir object.
    sb.ninodes = 200;
    super_block.vfs_sb = sb;
    // Inode initializing

    // To keep consistency, we write the super block to the disk and sets the
    // table state. This part should be read from the device and be created
    // when initializing the disk.
    for (uint i = 0; i < max_objects(); ++i) {
        objects_table_entry(i)->occupied = 0;
    }
    write_super_block();
    initialize_super_block_entry();
    initialize_objects_table_entry();
}


uint add_object(const void* object, uint size, const char* name) {
    uint err = check_add_object_validality(size, name);
    if (err != NO_ERR) {
        return err;
    }
    acquiresleep(&disklock);
    for (uint i = 0; i < max_objects(); ++i) {
        ObjectsTableEntry* entry = objects_table_entry(i);
        if (!entry->occupied) {
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
    }
    releasesleep(&disklock);
    return OBJECTS_TABLE_FULL;
}


uint rewrite_object(const void* object, uint size, const char* name) {
    uint err;
    err = check_rewrite_object_validality(size, name);
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
    super_block.bytes_occupied -= entry->size;
    if (entry->size >= size) {
        void* address =
            (void*)memory_storage + entry->disk_offset;
        memmove(address, object, size);
        entry->size = size;
    } else {
        entry->occupied = 0;
        super_block.occupied_objects -= 1;
        void* address = find_empty_space(size);
        entry->occupied = 1;
        super_block.occupied_objects += 1;
        if (!address) {
            releasesleep(&disklock);
            return NO_DISK_SPACE_FOUND;
        }
        memmove(address, object, size);
        entry->size = size;
        entry->disk_offset = address - (void*)memory_storage;
    }
    super_block.bytes_occupied += size;
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


uint get_object(const char* name, void* output) {
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
    void* address = (void*)memory_storage + entry->disk_offset;
    memmove(output, address, entry->size);

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
    for (uint i = 0; i < max_objects(); ++i) {
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


uint max_objects() {
    return super_block.objects_table_size;
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


uint device_size() {
    return super_block.storage_device_size;
}


uint occupied_bytes() {
    return super_block.bytes_occupied;
}
