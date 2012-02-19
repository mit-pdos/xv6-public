#define IPC_NMSG 16
typedef u32 msgid_t;
#define NULL_MSGID (-1)

#define IPC_NPAGE ((KSHAREDSIZE/PGSIZE) - 1)
typedef u32 pageid_t;
#define NULL_PAGEID (-1)

struct ipcmsg {
  volatile char done:1;
  volatile char submitted:1;

  pageid_t pageid;
  off_t off;
  volatile long result;
};

struct ipcctl {
  int msghead;
  int msgtail;
  struct ipcmsg msg[IPC_NMSG];

  int pagehead;
  int pagetail;
};
