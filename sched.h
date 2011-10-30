void delrun(struct proc*);
void initsched(void);
struct proc* schednext(void);
int steal(void);
void addrun(struct proc*);
