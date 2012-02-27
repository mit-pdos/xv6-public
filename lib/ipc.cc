#include "types.h"
#include "user.h"
#include "ipc.hh"

struct ipcctl *ipcctl = (struct ipcctl*)KSHARED;

msgid_t
ipc_msg_alloc(void)
{
  if (ipcctl->msghead - ipcctl->msgtail == IPC_NMSG)
    return nullptr_MSGID;

  msgid_t i = ipcctl->msghead % IPC_NMSG;
  ipcctl->msghead++;
  return i;
}

void
ipc_msg_free(int msgid)
{
  msgid_t i;

  i = ipcctl->msgtail % IPC_NMSG;
  if (i != msgid)
    die("ipc_free_msg: oops %u %u", i, msgid);
  ipcctl->msgtail++;
}

pageid_t
ipc_page_alloc(void)
{
  if (ipcctl->pagehead - ipcctl->pagetail == IPC_NPAGE)
    return nullptr_PAGEID;

  pageid_t i = ipcctl->pagehead % IPC_NPAGE;
  ipcctl->pagehead++;
  return i;
}

void
ipc_page_free(pageid_t pageid)
{
  pageid_t i;

  i = ipcctl->pagetail % IPC_NPAGE;
  if (i != pageid)
    die("ipc_free_page: oops");
  ipcctl->pagetail++;
}
