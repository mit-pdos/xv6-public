
#include "defs.h"
#include"MALLOC.h"

//#define Null ((void*)0)
enum Color {Red,Black};
void Print(struct proc *p){
    cprintf("The name is %s\n",p->name);
}
struct node{
    struct node *left,*right,*parent;
    struct proc *Process;
    enum Color node_color;
};
/*struct node *Create_Red_Black_Tree(struct proc* p){
    struct node *New=(struct node *)malloc(sizeof(struct node));
    New->node_color=Red;
    New->Process=p;
    New->left=Null;
    New->right=Null;
    New->parent=Null;
}*/