
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include"mmu.h"
#include"spinlock.h"
#include"proc.h"

#define Null ((void*)0)
enum Color {Red,Black};
void Print(struct proc *p){
    cprintf("The name is %s\n",p->name);
}
struct node{
    struct node *left,*right,*parent;
    struct node *Next;
    struct proc *Process;
    enum Color node_color;
};
struct node *Create_Red_Black_Tree(struct proc* p){
    struct node *New=(struct node *)kalloc();
    New->node_color=Red;
    New->Process=p;
    New->left=Null;
    New->right=Null;
    New->parent=Null;
    New->Next=New+sizeof(struct node);
    return New;
}
struct node *Create_Node(struct proc* p,struct node *x){
    struct node *New=x;
    New->node_color=Red;
    New->Process=p;
    New->left=Null;
    New->right=Null;
    New->parent=Null;
    New->Next=New+sizeof(struct node);   
    return New;
}