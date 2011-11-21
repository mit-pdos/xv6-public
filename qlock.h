struct qnode {
  volatile void *next;
  volatile char locked;
  __padout__;
};

typedef struct {
  struct qnode *v __mpalign__;
} qlock_t;

static inline void
ql_init(qlock_t *l)
{
        l->v = NULL;
}

static inline void
ql_lock(qlock_t *l, volatile struct qnode *mynode)
{
  struct qnode *predecessor;

  mynode->next = NULL;
  predecessor = (struct qnode *)xchg((u64 *)&l->v, (u64)mynode);

  if (predecessor) {
    mynode->locked = 1;
    barrier();
    predecessor->next = mynode;
    while (mynode->locked)
      rep_nop();
  }
}

static inline int
ql_trylock(qlock_t *l, volatile struct qnode *mynode)
{
  mynode->next = NULL;
  return cmpswap((long *)&l->v, 0, (long)mynode);
}

static inline void
ql_unlock(qlock_t *l, volatile struct qnode *mynode)
{
  if (!mynode->next) {
    if (cmpswap((long *)&l->v, (long)mynode, 0))
      return;
    while (!mynode->next)
      rep_nop();
  }
  ((struct qnode *)mynode->next)->locked = 0;
}
