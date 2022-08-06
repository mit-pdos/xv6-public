#include "obj_log.h"

#include "sleeplock.h"

#include "obj_fs.h"
#include "obj_cache.h"
#include "obj_disk.h"
#include "kvector.h"
#ifndef KERNEL_TESTS
#include "defs.h"  // import `panic`
#else
#include "obj_fs_tests_utilities.h"  // impot mock `panic`
#endif

/**
 * The loglock protect the logbook from multiple actions occuring at the same
 * time. Before every `panic`, we release the lock. By that, the
 * implementation does not depend on "panic" implementation.
 * Note that if panic does not stop the run, the behavior is undefined.
 * This feature lets us unit test edge cases which leads to panic.
 */
struct sleeplock loglock;

struct add_object_event {
    char object_id[MAX_OBJECT_NAME_LENGTH];
    uint total_objects_before;
    uint total_objects_after;
};

struct rewrite_object_event {
    char old_object_id[MAX_OBJECT_NAME_LENGTH];
    uint old_object_table_index;
    char new_object_id[MAX_OBJECT_NAME_LENGTH];
    uint new_object_table_index;
    uint total_objects;
};

struct delete_object_event {
    char object_id[MAX_OBJECT_NAME_LENGTH];
    uint total_objects_before;
    uint total_objects_after;
};

enum event_type {
    ADD_EVENT,
    REWRITE_EVENT,
    DELETE_EVENT
};

/**
 * As documented, the logbook contains only 1 entry.
 */
struct {
    enum event_type type;
    union {
        struct add_object_event add_event;
        struct rewrite_object_event rewrite_event;
        struct delete_object_event delete_event;
    };
} logbook;


// The caller must hold the log lock
static void finish_add_event() {
    uint size;
    if (cache_object_size(logbook.add_event.object_id, &size) != NO_ERR) {
        // The object was not created. Delete the event.
        cache_delete_object(LOGBOOK_OBJECT_ID);
        return;
    }
    if (occupied_objects() == logbook.add_event.total_objects_before) {
        set_occupied_objects(logbook.add_event.total_objects_after);
    } else if (occupied_objects() != logbook.add_event.total_objects_after) {
        releasesleep(&loglock);
        panic("unexpected occupied objects value");
    }
    cache_delete_object(LOGBOOK_OBJECT_ID);
}


// The caller must hold the log lock
static void finish_rewrite_event() {
    uint size;
    if (cache_object_size(logbook.rewrite_event.new_object_id, &size) != NO_ERR) {
        // The object was not created. Delete the event.
        cache_delete_object(LOGBOOK_OBJECT_ID);
        return;
    }
    uint err;
    if (logbook.rewrite_event.new_object_table_index == -1) {
        // update the new object table index
        err = get_objects_table_index(
            logbook.rewrite_event.new_object_id,
            &logbook.rewrite_event.new_object_table_index
        );
        if (err != NO_ERR) {
            releasesleep(&loglock);
            panic("logbook - error getting the new object objects table offset");
        }
        vector templogbookvector;
        templogbookvector = newvector(sizeof(logbook), 1);
        memmove_into_vector_bytes(templogbookvector,0,(char*)&logbook,sizeof(logbook));
        err = cache_rewrite_entire_object(templogbookvector, sizeof(logbook), LOGBOOK_OBJECT_ID);
        if (err != NO_ERR) {
            releasesleep(&loglock);
            panic("logbook - error updating the logbook to the disk");
        }
        freevector(&templogbookvector);
    }
    // change the new object id inside the table
    memmove(
        objects_table_entry(logbook.rewrite_event.new_object_table_index)->object_id,
        logbook.rewrite_event.old_object_id,
        obj_id_bytes(logbook.rewrite_event.old_object_id)
    );
    flush_objects_table_entry(logbook.rewrite_event.new_object_table_index);
    // delete the old object from the table
    objects_table_entry(logbook.rewrite_event.old_object_table_index)->occupied = 0;
    flush_objects_table_entry(logbook.rewrite_event.old_object_table_index);
    set_occupied_objects(occupied_objects() - 1);
    // delete the event
    if (cache_delete_object(LOGBOOK_OBJECT_ID) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when deleting the event object");
    }
    cache_free_from_cache_safe(logbook.rewrite_event.old_object_id);
}


// The caller must hold the log lock
static void finish_delete_event() {
    uint size;
    if (cache_object_size(logbook.delete_event.object_id, &size) == NO_ERR) {
        cache_delete_object(logbook.delete_event.object_id);
    }
    if (occupied_objects() == logbook.delete_event.total_objects_before) {
        set_occupied_objects(logbook.delete_event.total_objects_after);
    } else if (occupied_objects() != logbook.delete_event.total_objects_after) {
        releasesleep(&loglock);
        panic("unexpected occupied objects value");
    }
    cache_delete_object(LOGBOOK_OBJECT_ID);
}


// The caller must hold the log lock - if success, the lock is still held
static void finish_log_transactions() {
    uint event_size;
    uint err;
    err = object_size(LOGBOOK_OBJECT_ID, &event_size);
    if (err == OBJECT_NOT_EXISTS) {
        return;
    }
    if (err != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when loading the logbook event object size");
    }
    vector logbookvector;
    err = cache_get_object(LOGBOOK_OBJECT_ID, &logbookvector, 0);
    //vectormemcmp("finish_log_transactions", logbookvector, 0, (char*)&logbook, logbookvector.vectorsize);
    if (err != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when loading the logbook event object");
    }
    memmove_from_vector((char*)&logbook, logbookvector, 0, logbookvector.vectorsize);
    switch (logbook.type) {
        case ADD_EVENT:
            finish_add_event();
            break;
        case REWRITE_EVENT:
            finish_rewrite_event();
            break;
        case DELETE_EVENT:
            finish_delete_event();
            break;
        default:
            releasesleep(&loglock);
            panic("logbook - unexpected event type found inside logbook");
    }
}


void init_objfs_log() {
    initsleeplock(&loglock, "loglock");
    acquiresleep(&loglock);
    finish_log_transactions();
    releasesleep(&loglock);
}


uint log_add_object(const void* object, uint size, const char* name) {
    acquiresleep(&loglock);
    uint err = check_add_object_validality(size, name);
    if (err != NO_ERR) {
        releasesleep(&loglock);
        return err;
    }
    logbook.type = ADD_EVENT;
    memmove(logbook.add_event.object_id, name, obj_id_bytes(name));
    logbook.add_event.total_objects_before = occupied_objects();
    logbook.add_event.total_objects_after = occupied_objects() + 1;
    if (cache_add_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when adding the logbook object");
    }
    if (cache_add_object(object, size, name) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when adding the object itself");
    }
    if (cache_delete_object(LOGBOOK_OBJECT_ID) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when deleting the event object");
    }
    releasesleep(&loglock);
    return NO_ERR;
}

uint log_rewrite_object(const void* object, uint size, const char* name) {
    struct obj_dinode * di;

    acquiresleep(&loglock);
    uint err;
    err = check_rewrite_object_validality(size, name);
    if (err != NO_ERR) {
        releasesleep(&loglock);
        return err;
    }
    // write the event to the disk
    logbook.type = REWRITE_EVENT;
    memmove(logbook.rewrite_event.old_object_id, name, obj_id_bytes(name));
    memmove(logbook.rewrite_event.new_object_id, name, obj_id_bytes(name));
    logbook.rewrite_event.new_object_id[0] ^= 0xff;
    err = get_objects_table_index(
        name,
        &logbook.rewrite_event.old_object_table_index
    );
    if (err != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - error while getting the old object table index");
    }
    logbook.rewrite_event.new_object_table_index = -1;
    logbook.rewrite_event.total_objects = occupied_objects();
    err = cache_add_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID);
    if (err != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - error adding the logbook to the disk");
    }
    // adds the new object to the disk
    err = cache_add_object(object, size, logbook.rewrite_event.new_object_id);
    if (err != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - error adding the new object to the disk");
    }

    if (size == sizeof(*di)) {
        uint size_new = 0;
        uint size_old = 0;
        if (cache_object_size(logbook.rewrite_event.new_object_id, &size_new) != NO_ERR) {
            panic("in log rewrite failed to get size");
        };
        if (cache_object_size(logbook.rewrite_event.old_object_id, &size_old) != NO_ERR) {
            panic("in log rewrite failed to get size");
        };
    }
    finish_rewrite_event();
    releasesleep(&loglock);
    return NO_ERR;
}


uint log_delete_object(const char* name) {
    acquiresleep(&loglock);
    uint err = check_delete_object_validality(name);
    if (err != NO_ERR) {
        releasesleep(&loglock);
        return err;
    }
    logbook.type = DELETE_EVENT;
    memmove(logbook.delete_event.object_id, name, obj_id_bytes(name));
    logbook.delete_event.total_objects_before = occupied_objects();
    logbook.delete_event.total_objects_after = occupied_objects() - 1;
    if (cache_add_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when adding the logbook object");
    }
    if (cache_delete_object(name) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when deleting the object itself");
    }
    if (cache_delete_object(LOGBOOK_OBJECT_ID) != NO_ERR) {
        releasesleep(&loglock);
        panic("logbook - unexpected error when deleting the event object");
    }
    releasesleep(&loglock);
    return NO_ERR;
}

uint log_get_object(const char* name, vector * outputvector, uint read_object_from_offset) {
    acquiresleep(&loglock);
    uint result = cache_get_object(name, outputvector, read_object_from_offset);
    releasesleep(&loglock);
    return result;
}

uint log_object_size(const char* name, uint* output) {
    acquiresleep(&loglock);
    uint result = cache_object_size(name, output);
    releasesleep(&loglock);
    return result;
}
