#ifndef XV6_OBJ_CACHE_H
#define XV6_OBJ_CACHE_H

/**
 * `obj_cache` specify the second layer in the file system. It is the objects
 * cache layer. The cache in this file-system is different from the original
 * one written in xv6. The cache keep full objects that are frequently used.
 * In this project we implement LRU cache.
 *
 * When working with the cache, reading and writing objects takes more RAM
 * actions. For example, when loading an object from the disk we copy it both
 * for the user's buffer and for the cache buffer. Altough we does double the
 * work, it's performance is in orders of magnitude faster.
 *
 * The cache model has couple of limitaions:
 *  - The first one is the size of the objects. In the file system, there can
 *    be objects, for example, for size 100 GB. If we would try to cache them
 *    in the memory we would use 100 GB of the machine RAM which which is
 *    unacceptable for cache. In addition, the chance that someone would read
 *    the same object twice is almost zero. Hence, we limit the size of the
 *    objects we cache.
 *  - The cache memory is preallocated in the kernel data segment. Hence, the
 *    maximum size if fixed and can't be changed during the run.
 *  - To make the implementation simple for now, we don't dynamicly allocate
 *    space for objects in the cache. Instead, we have fixed size of entries
 *    in the cache for objects and each entry has maximum size. By that, the
 *    cache might have a lot of unused space. This behavior can be changed
 *    later by implementing a mechanism like in glibc's malloc.
 *
 * The regular file system's buffer cache is implemented in `bio.c`. The major
 * different between the 2 implementations is that in the regular fs, the data
 * is not copied to the user. Instead, he receive a pointer to the buffer. It
 * is possible because each buffer has the same size. In the object fs, each
 * object has different size and we set an upper size limitation. Hence, not
 * all of the objects would be cached. Hence, this layer is simpler than the
 * other one and doesn't have "transactions". If the user wants to write
 * multiple times to the buffer, he doesn't need to start a transaction, do
 * the writes and then call `brelse`. Instead, he has a copy of the objects
 * and he can edit it as much as he wants. In the end, he just write it back.
 *
 * Altoguh it looks like the cache layer improve the performance less than in
 * the regular file system case, it is still important. The performance
 * optimization comes when reading an objects that were already read or
 * objects that were added/updated and then read again. In these cases, the
 * cache remove the need to read the data again from the disk.
 *
 * In each of the add/rewrite/get and object_size methods, the object is
 * cached for further use. In addition, in the `delete_object` method, if
 * the object is located in the cache, it is "freed" from it. In that case
 * we specify that this buffer is the oldest buffer that was in use.
 *
 * Implementation details:
 * ~~~~~~~~~~~~~~~~~~~~~~~
 * In similarity to the implementation in `bio.c`, we use a bi-directional
 * linked list to keep the buffer. The head is the most recently used object
 * and the tail is the oldest. The oldest is reached by `head->prev`.
 * If in any of the functions, an error is raised, then the cache state does
 * not change. For example, if object addition fails because the storage
 * device is full, then the object isn't added to the cache either.
 *
 *
 * Implementation extensions:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1. In the current version, the disk is NOT thread safe.
 *      TODO make it a thread safe.
 * 2. The cache currently supports only 1 device in any given time.
 *      TODO support more devices.
 */


#include "types.h"
#include "kvector.h"


void init_objects_cache();

void _check(char * val);
uint cache_add_object(const void* object, uint size, const char* name);
uint cache_rewrite_entire_object(vector object, uint size, const char* name); 
uint cache_rewrite_object(vector data, uint objectsize, uint offset, const char * name);
uint cache_delete_object(const char* name); 
uint cache_object_size(const char* name, uint* output);
uint cache_get_object(const char* name, vector * outputvector, uint read_object_from_offset);

/**
 * Remove the object from the objects cache but not form the disk.
 * This function locks the cache lock and release it in the end.
 */
uint cache_free_from_cache_safe(const char* name);


/**
 * The following methods provides statistics about the cache layer. They can
 * used by program to show performance of the file system or to try and
 * optimize their run flow.
 */

uint objects_cache_hits();
uint objects_cache_misses();
uint cache_max_object_size();

#endif /* XV6_OBJ_CACHE_H */

