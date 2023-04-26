#ifndef XV6_KVEC_H
#define XV6_KVEC_H

#define getelement(vec, ind, typ) *(typ*)getelementpointer(vec,ind)

typedef struct kvec {
    unsigned int vectorsize;            // Number of elements in the vector
    unsigned int typesize;              // The size of each element measured in bytes
    char* head;                         // A pointer to the first link(array) of the vector
    char* tail;                         // A pointer to the last link of the vector
    char* lastaccessed;                 // A pointer to the last link accessed
    unsigned int lastindexaccessed;     // The index of the first link accessed (counting from 0)
    int valid;                          // 1 if the initialization function succeeds. 0 if it fails.
} vector;

#endif /* XV6_KVEC_H */

