#ifndef KVEC_H
#define KVEC_H

#define getelement(vec, ind, typ) *(typ*)getelementpointer(vec,ind)

typedef struct kvec {
    unsigned int vectorsize;        // number of elements in the kvector
    unsigned int typesize;          // the size of each element in the kvector (byts)
    char* head;                     // a pointer to the first link it that list
    char* tail;                     // a pointer to the last link in that list
    char* lastaccessed;             // cashing: last accessed kvector link
    unsigned int lastindexaccessed; // cashing: last accessed kvector link index
    int valid;                      // indicates kvec validity
} vector;

#endif