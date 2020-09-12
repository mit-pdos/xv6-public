#include "obj_log.h"

#include "string.h"

#include "obj_cache.h"
#include "obj_disk.h"
#ifndef KERNEL_TESTS
#include "defs.h"  // import `panic`
#else
#include "obj_fs_tests_utilities.h"  // impot mock `panic`
#endif


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
        panic("unexpected occupied objects value");
    }
    cache_delete_object(LOGBOOK_OBJECT_ID);
}


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
            panic("logbook - error getting the new object objects table offset");
        }
        err = cache_rewrite_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID);
        if (err != NO_ERR) {
            panic("logbook - error updating the logbook to the disk");
        }
    }
    // change the new object id inside the table
    memcpy(
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
        panic("logbook - unexpected error when deleting the event object");
    }
    cache_free_from_cache(logbook.rewrite_event.old_object_id);
}


static void finish_delete_event() {
    uint size;
    if (cache_object_size(logbook.delete_event.object_id, &size) == NO_ERR) {
        cache_delete_object(logbook.delete_event.object_id);
    }
    if (occupied_objects() == logbook.delete_event.total_objects_before) {
        set_occupied_objects(logbook.delete_event.total_objects_after);
    } else if (occupied_objects() != logbook.delete_event.total_objects_after) {
        panic("unexpected occupied objects value");
    }
    cache_delete_object(LOGBOOK_OBJECT_ID);
}


void finish_log_transactions() {
    uint event_size;
    uint err;
    err = object_size(LOGBOOK_OBJECT_ID, &event_size);
    if (err == OBJECT_NOT_EXISTS) {
        return;
    }
    if (err != NO_ERR) {
        panic("logbook - unexpected error when loading the logbook event object size");
    }
    err = cache_get_object(LOGBOOK_OBJECT_ID, &logbook);
    if (err != NO_ERR) {
        panic("logbook - unexpected error when loading the logbook event object");
    }
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
            panic("logbook - unexpected event type found inside logbook");
    }
}


uint log_add_object(const void* object, uint size, const char* name) {
    uint err = check_add_object_validality(size, name);
    if (err != NO_ERR) {
        return err;
    }
    logbook.type = ADD_EVENT;
    memcpy(logbook.add_event.object_id, name, obj_id_bytes(name));
    logbook.add_event.total_objects_before = occupied_objects();
    logbook.add_event.total_objects_after = occupied_objects() + 1;
    if (cache_add_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID) != NO_ERR) {
        panic("logbook - unexpected error when adding the logbook object");
    }
    if (cache_add_object(object, size, name) != NO_ERR) {
        panic("logbook - unexpected error when adding the object itself");
    }
    if (cache_delete_object(LOGBOOK_OBJECT_ID) != NO_ERR) {
        panic("logbook - unexpected error when deleting the event object");
    }
    return NO_ERR;
}


uint log_rewrite_object(const void* object, uint size, const char* name) {
    uint err;
    err = check_rewrite_object_validality(size, name);
    if (err != NO_ERR) {
        return err;
    }
    // write the event to the disk
    logbook.type = REWRITE_EVENT;
    memcpy(logbook.rewrite_event.old_object_id, name, obj_id_bytes(name));
    memcpy(logbook.rewrite_event.new_object_id, name, obj_id_bytes(name));
    logbook.rewrite_event.new_object_id[0] ^= 0xff;
    err = get_objects_table_index(
        name,
        &logbook.rewrite_event.old_object_table_index
    );
    if (err != NO_ERR) {
        panic("logbook - error while getting the old object table index");
    }
    logbook.rewrite_event.new_object_table_index = -1;
    logbook.rewrite_event.total_objects = occupied_objects();
    err = cache_add_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID);
    if (err != NO_ERR) {
        panic("logbook - error adding the logbook to the disk");
    }
    // adds the new object to the disk
    err = cache_add_object(object, size, logbook.rewrite_event.new_object_id);
    if (err != NO_ERR) {
        panic("logbook - error adding the new object to the disk");
    }
    finish_rewrite_event();
    return NO_ERR;
}


uint log_delete_object(const char* name) {
    uint err = check_delete_object_validality(name);
    if (err != NO_ERR) {
        return err;
    }
    logbook.type = DELETE_EVENT;
    memcpy(logbook.delete_event.object_id, name, obj_id_bytes(name));
    logbook.delete_event.total_objects_before = occupied_objects();
    logbook.delete_event.total_objects_after = occupied_objects() - 1;
    if (cache_add_object(&logbook, sizeof(logbook), LOGBOOK_OBJECT_ID) != NO_ERR) {
        panic("logbook - unexpected error when adding the logbook object");
    }
    if (cache_delete_object(name) != NO_ERR) {
        panic("logbook - unexpected error when deleting the object itself");
    }
    if (cache_delete_object(LOGBOOK_OBJECT_ID) != NO_ERR) {
        panic("logbook - unexpected error when deleting the event object");
    }
    return NO_ERR;
}
