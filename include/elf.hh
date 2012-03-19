// Format of an ELF executable file
// From linux/include/linux/elf.h

#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

typedef u64 Elf64_Addr;
typedef u64 Elf64_Off;
typedef u16 Elf64_Half;
typedef u32 Elf64_Word;
typedef s32 Elf64_Sword;
typedef u64 Elf64_Xword;
typedef s64 Elf64_Sxword;
typedef s16 Elf64_Section;

struct elfhdr {
  Elf64_Word magic;             // Must equal ELF_MAGIC
  u8 ident[12];
  Elf64_Half machine;
  Elf64_Word version;
  Elf64_Addr entry;             // Entry point virtual address
  Elf64_Off phoff;              // Program header table file offset
  Elf64_Off shoff;              // Section header table file offset
  Elf64_Word flags;
  Elf64_Half ehsize;
  Elf64_Half phentsize;
  Elf64_Half phnum;
  Elf64_Half shentsize;
  Elf64_Half shnum;
  Elf64_Half shstrndx;
};

struct proghdr {
  Elf64_Word type;
  Elf64_Word flags;
  Elf64_Off offset;             // Segment file offset
  Elf64_Addr vaddr;             // Segment virtual address
  Elf64_Addr paddr;             // Segment physical address
  Elf64_Xword filesz;           // Segment size in file
  Elf64_Xword memsz;            // Segment size in memory
  Elf64_Xword align;            // Segment alignment, file & memory
};

struct elfnote {
  Elf64_Word namesz;            // Name size
  Elf64_Word descsz;	        // Content size
  Elf64_Word type;	        // Content type
};

// Values for Proghdr type
#define ELF_PROG_LOAD           1
#define ELF_PROG_NOTE           4

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

// All known .note types
#define ELF_NOTE_XV6_ADDR       1

// xv6-specific address note
struct xv6_addrdesc {
  Elf64_Word id;
  Elf64_Addr vaddr;
};

struct xv6_addrnote {
  struct elfnote elfnote;
  // name is 0 bytes
  struct xv6_addrdesc desc;
};

// All xv6-specific IDs for notes about addresses
#define XV6_ADDR_ID_WQ 1

#define DEFINE_XV6_ADDRNOTE(xname, xid, xvaddr)                         \
  const struct xv6_addrnote xname PROG_NOTE_ATTRIBUTE = {               \
    elfnote: {                                                          \
      namesz: 0,                                                        \
      descsz: sizeof(((xv6_addrnote *)nullptr)->desc),                  \
      type: ELF_NOTE_XV6_ADDR                                           \
    },                                                                  \
    desc: {                                                             \
      id: (xid),                                                        \
      vaddr: (Elf64_Addr)(xvaddr) }                                     \
  }

#define PROG_NOTE_ATTRIBUTE __attribute__ ((section(".note"), used))
