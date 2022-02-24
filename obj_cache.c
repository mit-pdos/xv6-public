#include "string.h"

#include "obj_cache.h"
#include "obj_disk.h"
#include "sleeplock.h"

#ifndef KERNEL_TESTS
#include "defs.h"  // import `panic`
#else
#include "obj_fs_tests_utilities.h"  // impot mock `panic`
#endif


// the default cache has 32 objects of 8KB each for total of 256KB.
#ifndef CACHE_MAX_OBJECT_SIZE
#define CACHE_MAX_OBJECT_SIZE 8192
#endif

#ifndef OBJECTS_CACHE_ENTRIES
#define OBJECTS_CACHE_ENTRIES 100
#endif


struct sleeplock cachelock;


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


void init_objects_cache() {
    hits = 0;
    misses = 0;

    initsleeplock(&cachelock, "cachelock");

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
    acquiresleep(&cachelock);
    uint rv = add_object(object, size, name);
    if (rv != NO_ERR) {
        releasesleep(&cachelock);
        return rv;
    }
    if (size > CACHE_MAX_OBJECT_SIZE) {
        releasesleep(&cachelock);
        return NO_ERR;
    }
    // The object is not located in the cache because it is new. And if it was
    // already created before, it's fair to assume the data is different.
    struct obj_cache_entry* e = obj_cache.head.prev;
    move_to_front(e);
    e->size = size;
    memmove(e->data, object, size);
    memmove(e->object_id, name, obj_id_bytes(name));
    misses++;
    releasesleep(&cachelock);
    return NO_ERR;
}

uint cache_rewrite_object(const void* object, uint size, const char* name) {
    acquiresleep(&cachelock);
    uint rv = rewrite_object(object, size, name);
    if (rv != NO_ERR) {
        releasesleep(&cachelock);
        return rv;
    }
    if (size > CACHE_MAX_OBJECT_SIZE) {
        releasesleep(&cachelock);
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
    memmove(e->data, object, size);
    memmove(e->object_id, name, obj_id_bytes(name));
    releasesleep(&cachelock);
    return NO_ERR;
}


/// the caller holds the cache lock
static uint cache_free_from_cache(const char* name) {
    // the object might be inside the cache and might not
    for(struct obj_cache_entry* e = obj_cache.head.prev;
        e != &obj_cache.head;
        e = e->prev)
    {
        if (obj_id_cmp(name, e->object_id) == 0) {
            move_to_back(e);
            e->object_id[0] = 0;
            // TODO: remove?
            releasesleep(&cachelock);
            return NO_ERR;
        }
    }
    return OBJECT_NOT_EXISTS;
}

uint cache_delete_object(const char* name) {
    acquiresleep(&cachelock);
    uint rv = delete_object(name);
    if (rv != NO_ERR) {
        releasesleep(&cachelock);
        return rv;
    }
    // "remove" the object from the cache - otherwise the assumption
    // in `cache_add_objet` won't be valid.
    rv = cache_free_from_cache(name);
    if (rv == OBJECT_NOT_EXISTS) {
        misses++;
    } else if (rv == NO_ERR) {
        hits++;
    } else {
        releasesleep(&cachelock);
        panic("unexpected error from cache_free_from_cache");
    }
    releasesleep(&cachelock);
    return NO_ERR;
}

uint cache_object_size(const char* name, uint* output) {
    acquiresleep(&cachelock);
    for(struct obj_cache_entry* e = obj_cache.head.prev;
        e != &obj_cache.head;
        e = e->prev)
    {
        if (obj_id_cmp(name, e->object_id) == 0) {
            *output = e->size;
            move_to_front(e);
            hits++;
            releasesleep(&cachelock);
            return NO_ERR;
        }
    }
    misses++;
    uint err = object_size(name, output);
    releasesleep(&cachelock);
    return err;
}

uint cache_get_object(const char* name, void* output) {
    acquiresleep(&cachelock);
    for(struct obj_cache_entry* e = obj_cache.head.prev;
        e != &obj_cache.head;
        e = e->prev)
    {
        if (obj_id_cmp(name, e->object_id) == 0) {
            memmove(output, e->data, e->size);
            move_to_front(e);
            hits++;
            releasesleep(&cachelock);
            return NO_ERR;
        }
    }
    misses++;
    uint rv = get_object(name, output);
    if (rv != NO_ERR) {
        releasesleep(&cachelock);
        return rv;
    }
    uint size;
    if (object_size(name, &size) != NO_ERR) {
        releasesleep(&cachelock);
        panic("cache get object failed to get object size");
    }
    if (size > CACHE_MAX_OBJECT_SIZE) {
        releasesleep(&cachelock);
        return NO_ERR;
    }
    struct obj_cache_entry* e = obj_cache.head.prev;
    move_to_front(e);
    e->size = size;
    if (get_object(name, e->data)) {
        releasesleep(&cachelock);
        panic("cache get object failed to get object data");
    }
    memmove(e->object_id, name, obj_id_bytes(name));
    releasesleep(&cachelock);
    return NO_ERR;
}


uint cache_free_from_cache_safe(const char* name) {
    acquiresleep(&cachelock);
    uint err = cache_free_from_cache(name);
    releasesleep(&cachelock);
    return err;
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
