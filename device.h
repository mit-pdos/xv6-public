#define LOOP_DEVICE_DEV (7)
#define DEV_TO_LOOP_DEVICE(dev) ((dev) & 0xffff)
#define LOOP_DEVICE_TO_DEV(ld) ((ld) | (LOOP_DEVICE_DEV << 16))
#define IS_LOOP_DEVICE(dev) (((dev) >> 16) == LOOP_DEVICE_DEV)