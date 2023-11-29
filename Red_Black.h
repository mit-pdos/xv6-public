
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
struct node *Min_Vruntime_Node(struct node *node) {
    struct node *current = node;
    while (current->left != Null)
        current = current->left;
    return current;
}
void F2B(struct node **root, struct node *x) {
    if (x == *root)
        return;
    struct node *sibling = Null;
    if (x->parent->left == x) {
        sibling = x->parent->right;
        if (sibling->node_color==Red) {
            sibling->node_color = Black;
            x->parent->node_color = Red;
            rotateLeft(root, x->parent);
            sibling = x->parent->right;
        }
        if ((sibling->left == Null || sibling->left->node_color == Black) && (sibling->right == Null || sibling->right->node_color == Black)) {
            sibling->node_color=Red;
            if (x->parent->node_color == Black)
                F2B(root, x->parent);
            else
                x->parent->node_color= Black;
        } else {
            if (sibling->right == Null || sibling->right->node_color == Black) {
                sibling->left->node_color = Black;
                sibling->node_color = Red;
                rotateRight(root, sibling);
                sibling = x->parent->right;
            }
            sibling->node_color = x->parent->node_color;
            x->parent->node_color = Black;
            sibling->right->node_color = Black;
            rotateLeft(root, x->parent);
        }
    } else {
        sibling = x->parent->left;
        if (sibling->node_color == Red) {
            sibling->node_color = Black;
            x->parent->node_color = Red;
            rotateRight(root, x->parent);
            sibling = x->parent->left;
        }
        if ((sibling->left == Null || sibling->left->node_color == Black) && (sibling->right == Null || sibling->right->node_color == Black)) {
            sibling->node_color=Red;
            if (x->parent->node_color== Black)
                F2B(root, x->parent);
            else
                x->parent->node_color=Black;
        } else {
            if (sibling->left == Null || sibling->left->node_color == Black) {
                sibling->right->node_color = Black;
                sibling->node_color = Red;
                rotateLeft(root, sibling);
                sibling = x->parent->left;
            }
            sibling->node_color= x->parent->node_color;
            x->parent->node_color = Black;
            sibling->left->node_color = Black;
            rotateRight(root, x->parent);
        }
    }
}
void Delete(struct node **root,struct proc *p) {
    struct node *z = Create_Node(p);
    struct node *x, *y;
    struct node *current = *root;

    while (current != Null) {
        if (current->Process->Vruntime == z->Process->Vruntime) {
            z = current;
            break;
        }

        if (current->Process->Vruntime < z->Process->Vruntime)
            current = current->right;
        else
            current = current->left;
            
    }
    if (z == Null) {
        cprintf("No process was found with the provided pid\n");
        return;
    }
    if (z->left == Null || z->right == Null)
        y = z;
    else {
        y = Min_Vruntime_Node(z->right);
        z->Process->Vruntime = y->Process->Vruntime;
    }
    if (y->left != Null){

        x = y->left;
    }
    else{
        x = y->right;
    }
    if(x!=Null)
       x->parent = y->parent;
    
    if (y->parent == Null)
        *root = x;
    else if (y == y->parent->left)
        y->parent->left = x;
    else
        y->parent->right = x;
    if (y->node_color == Black)
        F2B(root, x);
    //kfree((char*)y);
}
void Traverse(struct node *root) {
    if (root == Null)
        return;
    Traverse(root->left);
    cprintf("A process with name %s was found in the tree\n", root->Process->name);
    Traverse(root->right);
}