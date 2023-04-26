#ifndef XV6_DEVICE_H
#define XV6_DEVICE_H
#define LOOP_DEVICE_DEV (7)
#define OBJ_DEV (15)
#define DEV_TO_LOOP_DEVICE(dev) ((dev) & 0xffff)
#define DEV_TO_OBJ_DEVICE(dev) ((dev) & 0xffff)
#define LOOP_DEVICE_TO_DEV(ld) ((ld) | (LOOP_DEVICE_DEV << 16))
#define OBJ_TO_DEV(ld) ((ld) | (OBJ_DEV << 16))
#define IS_LOOP_DEVICE(dev) (((dev) >> 16) == LOOP_DEVICE_DEV)
#define IS_OBJ_DEVICE(dev) (((dev) >> 16) == OBJ_DEV)
#endif /* XV6_DEVICE_H */
 