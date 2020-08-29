#include <string.h>

#ifndef KERNEL_TESTS
#include "defs.h"  // import `panic`
#endif

#include "obj_cache.h"
#include "obj_disk.h"


// the default cache has 32 objects of 8KB each for total of 256KB.
#ifndef CACHE_MAX_OBJECT_SIZE
#define CACHE_MAX_OBJECT_SIZE 8192
#endif

#ifndef OBJECTS_CACHE_ENTRIES
#define OBJECTS_CACHE_ENTRIES 32
#endif


uint hits;
uint misses;

struct obj_cache_entry {
    uchar data[CACHE_MAX_OBJECT_SIZE];
    uint size;
    char object_id[OBJECT_ID_LENGTH];

    struct obj_cache_entry* prev;
    struct obj_cache_entry* next;
};

struct {
  struct obj_cache_entry entries[OBJECTS_CACHE_ENTRIES];

  // Linked list of all buffers, through prev/next.
  // head.next is most recently used. The head itself doesn't keep an object.
  struct obj_cache_entry head;
} obj_cache;


#ifdef KERNEL_TESTS
#include <stdio.h>
#include <stdlib.h>
void panic(const char* msg) {
    printf("kernel panic: %s\n", msg);
    exit(1);
}
#endif


void init_objects_cache() {
    hits = 0;
    misses = 0;

    // the following, was copied from `bio.c` with minor changes.
    struct obj_cache_entry* e;
    obj_cache.head.prev = &obj_cache.head;
    obj_cache.head.next = &obj_cache.head;
    for (e = obj_cache.entries; e < obj_cache.entries + OBJECTS_CACHE_ENTRIES; e++) {
        e->size = 0;
        e->object_id[0] = 0;  // empty string

        e->next = obj_cache.head.next;
        e->prev = &obj_cache.head;
        obj_cache.head.next->prev = e;
        obj_cache.head.next = e;
    }
}


static void move_to_front(struct obj_cache_entry* e) {
    e->next->prev = e->prev;
    e->prev->next = e->next;
    e->next = obj_cache.head.next;
    e->prev = &obj_cache.head;
    obj_cache.head.next->prev = e;
    obj_cache.head.next = e;
}


static void move_to_back(struct obj_cache_entry* e) {
    e->next->prev = e->prev;
    e->prev->next = e->next;
    e->next = &obj_cache.head;
    e->prev = obj_cache.head.prev;
    obj_cache.head.prev->next = e;
    obj_cache.head.prev = e;
}

uint cache_add_object(const void* object, uint size, const char* name) {
    uint rv = add_object(object, size, name);
    if (rv != NO_ERR) {
        return rv;
    }
    if (size > CACHE_MAX_OBJECT_SIZE) {
        return NO_ERR;
    }
    // The object is not located in the cache because it is new. And if it was
    // already created before, it's fair to assume the data is different.
    struct obj_cache_entry* e = obj_cache.head.prev;
    move_to_front(e);
    e->size = size;
    memcpy(e->data, object, size);
    memcpy(e->object_id, name, obj_id_bytes(name));
    misses++;
    return NO_ERR;
}

uint cache_rewrite_object(const void* object, uint size, const char* name) {
    uint rv = rewrite_object(object, size, name);
    if (rv != NO_ERR) {
        return rv;
    }
    if (size > CACHE_MAX_OBJECT_SIZE) {
        return NO_ERR;
    }
    // the object might be inside the cache and might not
    struct obj_cache_entry* e = obj_cache.head.prev;
    misses++;
    for(struct obj_cache_entry* current = obj_cache.head.prev;
        current != &obj_cache.head;
        current = current->prev)
    {
        if (obj_id_cmp(name, current->object_id) == 0) {
            e = current;
            misses--;
            hits++;
            break;
        }
    }
    move_to_front(e);
    e->size = size;
    memcpy(e->data, object, size);
    memcpy(e->object_id, name, obj_id_bytes(name));
    return NO_ERR;
}

uint cache_delete_object(const char* name) {
    uint rv = delete_object(name);
    if (rv != NO_ERR) {
        return rv;
    }
    // the object might be inside the cache and might not
    for(struct obj_cache_entry* e = obj_cache.head.prev;
        e != &obj_cache.head;
        e = e->prev)
    {
        if (obj_id_cmp(name, e->object_id) == 0) {
            move_to_back(e);
            // "remove" the object from the cache - otherwise the assumption
            // in `cache_add_objet` won't be valid.
            e->object_id[0] = 0;
            hits++;
            return NO_ERR;
        }
    }
    misses++;
    return NO_ERR;
}

uint cache_object_size(const char* name, uint* output) {
    for(struct obj_cache_entry* e = obj_cache.head.prev;
        e != &obj_cache.head;
        e = e->prev)
    {
        if (obj_id_cmp(name, e->object_id) == 0) {
            *output = e->size;
            move_to_front(e);
            hits++;
            return NO_ERR;
        }
    }
    misses++;
    return object_size(name, output);
}

uint cache_get_object(const char* name, void* output) {
    for(struct obj_cache_entry* e = obj_cache.head.prev;
        e != &obj_cache.head;
        e = e->prev)
    {
        if (obj_id_cmp(name, e->object_id) == 0) {
            memcpy(output, e->data, e->size);
            move_to_front(e);
            hits++;
            return NO_ERR;
        }
    }
    misses++;
    uint rv = get_object(name, output);
    if (rv != NO_ERR) {
        return rv;
    }
    uint size;
    if (object_size(name, &size) != NO_ERR) {
        panic("cache get object failed to get object size");
    }
    if (size > CACHE_MAX_OBJECT_SIZE) {
        return NO_ERR;
    }
    struct obj_cache_entry* e = obj_cache.head.prev;
    move_to_front(e);
    e->size = size;
    if (get_object(name, e->data)) {
        panic("cache get object failed to get object data");
    }
    memcpy(e->object_id, name, obj_id_bytes(name));
    return NO_ERR;
}


uint objects_cache_hits() {
    return hits;
}

uint objects_cache_misses() {
    return misses;
}

uint cache_max_object_size() {
    return CACHE_MAX_OBJECT_SIZE;
}
