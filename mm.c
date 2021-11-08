#define MAX_REGION_SIZE 32
typedef char bool;
struct shm_region {
    bool valid;
    int rc;
    int len;
    uint physical_pages[MAX_REGION_SIZE];
};

struct shm_region regions[32];

void *
GetSharedPage(int i, int len) {
    int key;
    int num_pages;
    argint(0, &key);
    argint(1, &num_pages);
    if (hashtable[(int)key % sizeof(hashtable)] == 0) {
        for (int a = 0; a < num_pages; a = a + 1) {
            hashtable[key % sizeof(hashtable)] = kalloc();
        }
    } else {
        cprintf("request to share memory not implemented yet");
    }
    //hash key and find correct bucket
    //if empty:
    //allocate new page (use kalloc?)
    //save virtual addr
    //if already in use:
    //append to bucket
    //allow process to read and write in same space (lookup)
    //retrieve virtual addr and save
    //return virtual addr
    //return NULL if no space
    // Allocate pages in the appropriate regions' pysical pages
    if (!regions[i].valid) {
        for (int j = 0; j < len; j++) {
            void *newpage = kalloc();                     // Get new page
            memset(newpage, 0, PGSIZE);                   // Zero out page
            regions[i].physical_pages[j] = V2P(newpage);  // Save new page
            cprintf("New page allocated at virtual %p, physical %p, index %d\n", newpage, V2P(newpage), j);
        }
        regions[i].valid = 1;
        regions[i].len = len;
    } else {
        if (regions[i].len != len)
            return (void *)-1;
        cprintf("Picked up old region index %d\n", i);
    }
    regions[i].rc += 1;

    // Find the index in the process
    struct proc *p = myproc();
    int shind = -1;
    for (int i = 0; i < 32; i++) {
        if (p->shm[i].id == -1) {
            shind = i;
            break;
        }
    }
    if (shind == -1)
        return (void *)-2;

    // Get the lowest virtual address space currently allocated
    void *va = (void *)KERNBASE - 1;
    for (int i = 0; i < 32; i++) {
        if (p->shm[i].id != -1 && (uint)(va) > (uint)(p->shm[i].va)) {
            va = p->shm[i].va;
        }
    }

    // Get va of new mapped pages
    va = (void *)va - (len * PGSIZE);
    p->shm[shind].va = va;
    p->shm[shind].id = i;

    // Map them in memory
    uint addr = (uint)va;
    for (int k = 0; k < regions[i].len; k++) {
        mappages(p->pgdir, (void *)(addr + (k * PGSIZE)), PGSIZE, regions[i].physical_pages[k], PTE_W | PTE_U);
        cprintf("page %d allocated at %p with size %d\n", addr + (k * PGSIZE));
        (void)addr;
    }

    // Debug
    cprintf("global regions table after allocation\n");
    for (int z = 0; z < MAX_REGION_SIZE; z++) {
        cprintf("[%d]: valid: %d, refcount: %d\n", z, regions[z].valid, regions[z].rc);
    }
    return va;
}

void free_region(int id) {
    regions[id].valid = 0;
    regions[id].rc = 0;
    for (int i = 0; i < regions[id].len; i++)
        kfree(P2V(regions[id].physical_pages[i]));
    regions[id].len = 0;
}