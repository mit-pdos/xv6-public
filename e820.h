#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3
#define E820_NVS	4

#define NE820           32

struct e820entry {
  u64 addr;      // start of memory segment
  u64 size;      // size of memory segment
  u32 type;      // type of memory segment
} __attribute__((packed));

extern struct e820entry e820[NE820];

u64 e820bytes(void);
u64 e820size(void *va);
void *e820next(void *va, u64 inc);
