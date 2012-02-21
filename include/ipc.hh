// XXX(sbw) add a memlayout.h?
#define KSHARED 0xFFFFF00000000000ull

typedef u32 pageid_t;
typedef u32 msgid_t;

#define IPC_CTLSIZE 4096
#define IPC_PGSIZE  4096
#define IPC_NMSG    16
#define NULL_MSGID  (-1)
#define NULL_PAGEID (-1)
#define IPC_NPAGE   ((KSHAREDSIZE/IPC_PGSIZE) - 1)

struct ipcmsg {
  volatile char done:1;
  volatile char submitted:1;

  pageid_t pageid;
  off_t off;
  volatile long result;
};

struct ipcctl {
  volatile int msghead;
  volatile int msgtail;
  struct ipcmsg msg[IPC_NMSG];

  volatile int pagehead;
  volatile int pagetail;
};
extern struct ipcctl *ipcctl;

msgid_t  ipc_msg_alloc(void);
void     ipc_msg_free(int msgid);
pageid_t ipc_page_alloc(void);
void     ipc_page_free(pageid_t pageid);

static inline struct ipcmsg*
getmsg(msgid_t id)
{
  return &ipcctl->msg[id];
}

static inline char*
getpage(pageid_t id)
{
  return (char*)(KSHARED+IPC_CTLSIZE+(id*IPC_PGSIZE));
}
