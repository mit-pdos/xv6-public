#ifndef XV6_OBJ_LOG_H
#define XV6_OBJ_LOG_H


/**
 * `obj_log.h` specify the third layer in the object file system. The purpose
 * of it is to ensure the validality of the disk in case of unexpected
 * shutdowns. For example, lets look at the following scenario:
 *   - Computer power cable is plugged out, thus shuting down the computer
 *     immediatly (for desktop PCs)
 *   - At this time, a request to write an object was made and only part of
 *     the object was written until this point. This may occuer if the write
 *     to the disk is done is chunks.
 *   - Now, the disk doens't have the valid state of the old obejct and
 *     doesn't have the valid state of the new one.
 * If the written object was critical, for example the super-block, the file
 * system might be not restorable.
 * The larger the object, the higher the risk of this occuer. In general,
 * objects can be of any size, but the drivers themselfs can't hold an
 * unlimited size buffer for write requests. Hence, the objects are sent in
 * parts. This functionality is implemented by the first layer - the driver
 * layer.
 * Note: because such events only occure when writing to the disk, reading
 * methods can be used directly and not though the logbook.
 *
 * As we mentioned in `obj_disk.h`, hardwares has limitation on the maximum
 * size a request can has. In most cases it is 4KB of bytes written. When
 * writing an object this size, the hardware insure that the disk stay in a
 * valid state before and after. Either everything changed or nothing changed.
 * There are multiple hardware implementations that can ensure this but they
 * are out of this scope. With this, the responsability of the operating
 * system is to ensure that when writing large objects, either the new object
 * is written or the old one is but either way the disk has a valid object.
 *
 * The implementation we describe in this file, and that was implemented is
 * very basic and naive. Currently the driver level is blocking and doesn't
 * use interrupts to communicate with the disk. Hence, we don't have a list
 * of requests to the disk. Once a request is made, it is being executed
 * immediatly. In addition, we assume the objects table is managed by the OS
 * and the hard-drive is only a flash drive. Hence, we have the flexibility
 * to choose when to write to the table object.
 *
 * xv6 comperassion
 * ================
 * In the original xv6 file system logbook, the developers supported caching
 * requests in the memory and flushing them to the disk when it's free - in
 * the background. For example, when writing a large object, which is splitted
 * into multiple sub-write operations, the kernel can schedule the user code
 * and send the parts to the driver each time it finishs a part and raise an
 * intterupt.
 *
 * xv6 logbook has a maximum size and thus limiting the IO operation maximum
 * size. In object file system, partial objects operation has no meanning
 * and respectively the functions `begin_op` and `end_op` has no useage.
 *
 * Implementation details
 * ======================
 * The implementation only supports 1 IO operation from start to finish. Hence
 * there is not list as the one in the original xv6 implementation. The
 * logbook only use 1 objet with a constant name. If it is exists, there is an
 * unfinished operation, and if it doesn' there is no such one.
 * 
 * Add object requests:
 *     The risk when adding an object is an unfinished object written to the
 *     disk. To solve this, we can implement the following logic in the driver
 *     level: Because the objects table is software implemented we can first
 *     write all the object data to the disk and only then add it to the
 *     objects table. Changing an entry in the objects table is done by a
 *     request shorter than 4KB. Hence, the hardware gurantee the correctness
 *     of the disk. Either the entry would change to point to the new object
 *     data and the object would exists or the object data would get lost.
 *     This action is the atomic action that gurentee the corectness.
 *
 *     In the current mock implementation we keep counter about the amount of
 *     objects allocated in the file-system. This must be done in another
 *     operation. To achieve this, we write to the logbook a "add-object"
 *     event which holds the following:
 *        - object id
 *        - counter before
 *        - counter after
 *     The full add object flow is:
 *       1. add the event to the logbook
 *       2. write the object data to the disk
 *       3. add the object to the table
 *       4. change the counter as written in the event in the logbook
 *       5. delete the entry from the logbook
 *     We will now look what happen if the OS crash in the middle or after
 *     and of the steps and how we can restore the state:
 *       1 - 3: The object was not written to the disk, we delete the event
 *         from the logbook and the object is lost. The check can be done by
 *         searching the object id in the table.
 *       4. The object exists in the table, hence it was created. If the
 *         counter still has the old value then step 4 was not finished. We
 *         increment it to the new one and continue to step 5.
 *       5. If the counter in the disk has the new value as written in the
 *         event, we only need to delete it from the logbook.
 *     Deleting an object from the logbook is done by deleting any other
 *     object and is a safe operation as described below.
 *
 * Rewrite object requests:
 *     Rewriting an object is done in a similar method done by the xv6 logbook
 *     version. We use similar methods as in the add object operation. Yet, it
 *     has more complexity. Naive implementation would first copy the new
 *     object to a different location in the disk and then override the
 *     old one with the new data. The problem with this method is that we
 *     write the new object twice. When the object is huge, this price is too
 *     much. Hence, we need a better mechanism. The one we implemented first
 *     adds the object to a new position in the disk and then re-route the
 *     objects table to point onto this object. By doing so, we edit the table
 *     itself with caution. This adds complexity as you can see in the
 *     following flow:
 *       1. Add an "rewrite-object" event to the logbook with the following
 *          data:
 *            - old object id
 *            - old object table entry index
 *            - new object id
 *            - new object table entry index - with default value (-1)
 *       2. Create a new object in the disk by using "add-object" with an id of
 *         "new object id". Copy the new requested data to this object.
 *       3. Update the event object in the disk to contain the new objects
 *         table entry index.
 *       4. Change the new object id to be the same as the old one. This
 *         action re-route the table to point to the new object.
 *       5. Delete the old object entry from the table by marking it free.
 *       6. Delete the event from the logbook.
 *       7. The object in the cache still has the old value, we free it from
 *         the cache.
 *     Note that at stage 5 the table is in corrupted state because there are
 *     2 objects with the same id.
 *     We now prove that if the OS crash in any of the steps, the FS can end in
 *     a valid state:
 *       - If the event object has not been created or it indeed been created
 *         but the "new object" didn't, delete the event from the logbook and
 *         the change is discarded.
 *       - If it has been created but we didn't update the new object table
 *         entry index, we can search it across the table and write its index
 *         to the disk.
 *       - From step 4 and forward, we can continue from the point we stopped.
 *         We can check if we finished step 3 by checking the new object table
 *         index inside the event.
 *
 * Delete object requests:
 *     Deleting an object is done by setting the "is entry occupied" field to
 *     0. This is done by changing the table entry which is less than 4KB.
 *     Hence, this can be done in 1 disk operation.
 *     Yet, we still need to reduce the allocated object counter in the super
 *     block by 1 if it has not occured. This is done as we done in the
 *     "add object" operation.
 *
 *
 * Error handling:
 * ===============
 * When writing an object to the disk, the error can finish in an error in the
 * OS level. For exmample, if the object already exists or the object name is
 * too long. This happen only when trying to execute the operation itself.
 * Thus, these checks must be done before adding the object to the logbook. Or
 * else, the kernel won't be able to give feedback about these errors. For
 * example, if it added the request to the logbook and the PC crashed. Then,
 * it tries to finish it when it gets up and got an error, who does it return
 * the value to?
 * Still, couple of errors can't be handle beforehand. They are the "table is
 * full" and "not enough disk space available". Think about "rewrite"
 * operation, in this one we first copy the object to another location and
 * only then reroute the table to point to the new location. In this caase,
 * checking before hand if the operation would end in one of these problems
 * would cost twice the overhead. Hence, we currently don't support these.
 * There are better algorithms to handle such errors but they are out of the
 * scope of this project.
 * Another error is "object does not exists" which currently cost
 * O(size of the objects table) and can be reduced to O(1) by using hash
 * function on the object id.
 *
 *
 * Futher improvments
 * ==================
 * In the current version, the disk is NOT thread safe.
 * TODO make it a thread safe.
 * Handle all the possible errors
 *   - full disk and full objects table
 *   - object does not exists
 * When rewriting an object we free it from the cache. Hence, it would be
 *  loaded the next time it is requested. BUT we could have save it when
 *  writing the new object to the disk.
 */

#include "types.h"
#include "kvector.h"

#define LOGBOOK_OBJECT_ID "\xfflogbook\xff"

void init_objfs_log();
uint log_add_object(const void* object, uint size, const char* name);
uint log_rewrite_object(const void* object, uint size, const char* name);
uint log_delete_object(const char* name);
uint log_get_object(const char* name, vector * outputvector, uint readfromoffset);
uint log_object_size(const char* name, uint* output);

#endif /* XV6_OBJ_LOG_H */

