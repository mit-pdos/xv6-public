
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
struct node *Create_Node(struct proc* p){
    struct node *New=(struct node *)kalloc();
    New->node_color=Red;
    New->Process=p;
    New->left=Null;
    New->right=Null;
    New->parent=Null;
    return New;
}
void rotateLeft(struct node **root, struct node *x) {
    struct node *y = x->right;
    x->right = y->left;
    if (y->left != Null)
        y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == Null)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}

void rotateRight(struct node **root, struct node *y) {
    struct node *x = y->left;
    y->left = x->right;
    if (x->right != Null)
        x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == Null)
        *root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    x->right = y;
    y->parent = x;
}
void FixViolation(struct node **root, struct node *New) {
    struct node *Parent = Null;
    struct node *GrandParent = Null;
    while ((New != *root) && (New->node_color != Black) && (New->parent->node_color == Red)) {
        Parent = New->parent;
        GrandParent = New->parent->parent;
        if (Parent == GrandParent->left) {
            struct node *Uncle = GrandParent->right;
            if (Uncle != Null && Uncle->node_color == Red) {
                GrandParent->node_color=Red;
                Parent->node_color=Black;
                Uncle->node_color=Black;
                New=GrandParent;
            } else {
                if (New == Parent->right) {
                    rotateLeft(root, Parent);
                    New=Parent;
                    Parent=New->parent;
                }
                rotateRight(root, GrandParent);
                int temp =Parent->node_color;
                Parent->node_color = GrandParent->node_color;
                GrandParent->node_color= temp;
                New= Parent;
            }
        } else {
            struct node *Uncle = GrandParent->left;
            if ((Uncle != Null) && (Uncle->node_color == Red)) {
                GrandParent->node_color = Red;
                Parent->node_color=Black;
                Uncle->node_color=Black;
                New = GrandParent;
            } else {
                if (New == Parent->left) {
                    rotateRight(root, Parent);
                    New=Parent;
                    Parent = New->parent;
                }
                rotateLeft(root, GrandParent);
                int temp = Parent->node_color;
                Parent->node_color = GrandParent->node_color;
                GrandParent->node_color= temp;
                New=Parent;
            }
        }
    }
    (*root)->node_color=Black;
}
void Insert(struct node **root,struct proc* p) {
    struct node *New = Create_Node(p);
    if (*root == Null) {
        New->node_color=Black;
        *root = New;
        return;
    }
    struct node *Current = *root;
    struct node *Parent = Null;
    while (Current != Null) {
        Parent = Current;
        if (New->Process->Vruntime < Current->Process->Vruntime)
            Current = Current->left;
        else
            Current = Current->right;
    }
    New->parent = Parent;
    if (New->Process->Vruntime < Parent->Process->Vruntime)
        Parent->left =New;
    else
        Parent->right = New;
    FixViolation(root, New);
}
