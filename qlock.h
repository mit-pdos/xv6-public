struct qnode {
  volatile void *next;
  volatile char locked;
  __padout__;
};

typedef struct {
  struct qnode *v __mpalign__;
  const char *name;
} qlock_t;

static inline void
ql_init(qlock_t *l, const char *name)
{
  l->v = 0;
  l->name = name;
}

static inline void
ql_lock(qlock_t *l, volatile struct qnode *mynode)
{
  struct qnode *predecessor;

  pushcli();
  mtlock(l);
  mynode->next = NULL;
  predecessor = (struct qnode *)xchg((u64 *)&l->v, (u64)mynode);
  if (predecessor) {
    mynode->locked = 1;
    barrier();
    predecessor->next = mynode;
    while (mynode->locked)
      rep_nop();
  }
  mtacquired(l);
}

static inline int
ql_trylock(qlock_t *l, volatile struct qnode *mynode)
{
  int r;

  mynode->next = NULL;
  pushcli();
  r = cmpswap((long *)&l->v, 0, (long)mynode);
  if (!r)
    popcli();
  return r;
}

static inline void
ql_unlock(qlock_t *l, volatile struct qnode *mynode)
{
  if (!mynode->next) {
    if (cmpswap((long *)&l->v, (long)mynode, 0)) {
      mtunlock(l);
      popcli();
      return;
    }
    while (!mynode->next)
      rep_nop();
  }
  ((struct qnode *)mynode->next)->locked = 0;
  mtunlock(l);
  popcli();
}
