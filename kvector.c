/**************
 * This file is an implementaion of
 * the vector data structure.
 * It is basically a linked list of
 * arrays. 
 **************/
#include "types.h"
#include "mmu.h"
#include "kvector.h"

#ifndef UNITTESTS
#include "defs.h"
#else
#include "unittests/unitmock.h"
#include <string.h>
#endif


#define check_existence(vp, onerror) if((vp) == NULL) return onerror
#define check_validity(vp, onerror) if((vp)->valid == 0) return onerror

#define KVEC_ERR 0

//TODO:
// int addelement(vector v, char* data)
// int foreach(int (f*)(vector *, char*))
// int foreachinrange(unsigned int from, unsigned int to, int (f*)(vector *, char*))
// add small onepage cache just to enhance sequential access

// Segment Operations
// segment structure [prev | next | elements...]
void
setprev(char* sgmnt, char * prevpointer){
    char ** p = (char **)sgmnt;
    p[0] = prevpointer;
}

void
setnext(char* sgmnt, char * nextpointer){
    char ** p = (char **)sgmnt;
    p[1] = nextpointer;
}

char*
getprev(char* sgmnt){
    return ((char**)sgmnt)[0];
}

char*
getnext(char* sgmnt){
    return ((char**)sgmnt)[1];
}

void
getpageforindex(vector v, unsigned int index, unsigned int* page, unsigned int* offset){
    unsigned int pointersspace = 2*sizeof(char*);
    unsigned int elementsperpage = (PGSIZE - pointersspace) / v.typesize ;
    unsigned int pagenumber = index / elementsperpage;
    unsigned int pageoffset = index % elementsperpage;
    *page = pagenumber;
    *offset = pageoffset;  
}

unsigned int
countpages(vector v){
    unsigned int pg, offst;
    getpageforindex(v, v.vectorsize - 1, &pg, &offst);
    return (pg+1);
}

unsigned int
countactualpages(vector v){
    unsigned int counter = 0;
    char* currentpage = v.head;
    while(currentpage != NULL){
        currentpage = getnext(currentpage);
        counter++;
    }
    return counter;
}

// vector operations
char*
getelementpointer(vector v, unsigned int index){
    if(v.valid == 1 && v.vectorsize > index){
        unsigned int pageindex, pageoffset;
        getpageforindex(v, index, &pageindex, &pageoffset);
        int currentpageindex;
        char* currentpage = v.head;
        for(currentpageindex=0;currentpageindex < pageindex; currentpageindex++){
            currentpage = getnext(currentpage);
        }
        return &(currentpage[2*sizeof(char**) + pageoffset*v.typesize]);
    }else{
        //cprintf("RETURNED NULL WHEN: size: %d, actualsize : %d, valid: %d, index: %d\n", v.vectorsize,countactualpages(v),v.valid, index);
        return NULL;
    }
}

//void
//printsegment(char * sgmnt){
//    cprintf("[ %p | %p | <somecontent> ]\n", getprev(sgmnt) , getnext(sgmnt));
//}

unsigned int
setelement(vector v, unsigned int index, char* data){
    if(v.valid && v.vectorsize && v.vectorsize > index){
        unsigned int pageindex, pageoffset;
        getpageforindex(v, index, &pageindex, &pageoffset);
        int currentpageindex;
        char* currentpage = v.head;
        for(currentpageindex=0;currentpageindex < pageindex; currentpageindex++){
            currentpage = getnext(currentpage);
            
        }
        int currentbyteindex;
        for(currentbyteindex = 0; currentbyteindex < v.typesize; currentbyteindex++){
            currentpage[2*sizeof(char**) + pageoffset*v.typesize + currentbyteindex] = data[currentbyteindex];
        }
        return 1;
    }else{
        return 0;
    }
}

unsigned int
setbyte(vector v, unsigned int index, char* databyte){
    if(v.valid && (v.vectorsize * v.typesize) > index){
        unsigned int pageindex, pageoffset;

        unsigned int pointersspace = 2*sizeof(char*);
        unsigned int bytesperpage = (PGSIZE - pointersspace) ;
        pageindex = index / bytesperpage;
        pageoffset = index % bytesperpage;

        int currentpageindex;
        char* currentpage = v.head;
        for(currentpageindex=0;currentpageindex < pageindex; currentpageindex++){
            currentpage = getnext(currentpage);
        }
        //cprintf("current byte before: %c\n", currentpage[2*sizeof(char**) + pageoffset]);
        currentpage[2*sizeof(char**) + pageoffset] = *databyte;
        //cprintf("current byte after: %c\n", currentpage[2*sizeof(char**) + pageoffset]);
        return 1;
    }else{
        return 0;
    }
}

void
constructarray(char ** head, char ** tail, unsigned int numberofelements, unsigned int elementsize, int * error){
    unsigned int pointersspace = 2*sizeof(char*);
    unsigned int elementsperpage = (PGSIZE - pointersspace) / elementsize ;
    unsigned int requiredpages = numberofelements / elementsperpage + (numberofelements % elementsperpage != 0 ? 1 : 0);
    
    int currentpageindex;
    for(currentpageindex = 0; currentpageindex < requiredpages; currentpageindex++){
        char* p = kalloc();
        if(p != 0){
            switch (currentpageindex)
            {
            case 0: {
                setprev(p, NULL);
                *head = p;
            }break;
            default:{
                setprev(p, *tail);
                setnext(*tail, p);
            }break;
            }
            setnext(p, NULL);
            *tail = p;
        }else{
            *error = 1;
            return;
        }
    }
}

vector
newvector(unsigned int size, unsigned int typesize){
    vector v;
    v.vectorsize = size;
    v.typesize = typesize;
    v.valid=0;

    //caching
    v.lastaccessed = NULL;
    v.lastindexaccessed = -1;

    int error = 0;
    constructarray(&(v.head), &(v.tail), v.vectorsize, v.typesize, &error);

    if(!error)
        v.valid=1;
    return v;
}

void
freevector(vector * v){
    char* currentpage = v->head;
    while (currentpage != NULL){
        char * nextpage = getnext(currentpage);
        kfree(currentpage);
        currentpage = nextpage;
    }
    v->valid = 0;
    v->head = NULL;
    v->tail = NULL;
}

void
memmove_into_vector_bytes(vector dstvec,unsigned int dstbyteoffset, char* src, unsigned int size){
    int i;
    for(i=0;i<size;i++){
        setbyte(dstvec,i+dstbyteoffset,&(src[i]));
    }
}

void
memmove_into_vector_elements(vector dstvec, unsigned int dstelementoffset, char* src, unsigned int size){
    int i;
    for(i=0;i<size;i++){
        setelement(dstvec,i+dstelementoffset,&(src[i*dstvec.typesize]));
    }
}

void
memmove_from_vector(char * dst, vector vec, unsigned int elementoffset, unsigned int elementcount){
    int counter;
    for(counter = 0; counter < elementcount; counter++){
        memmove(dst + counter*vec.typesize , getelementpointer(vec, elementoffset + counter), vec.typesize);
    }
}

vector
slicevector(vector original, unsigned int startfrom, unsigned int inclusiveend){
    unsigned int curindex;
    unsigned int numberofelements = inclusiveend - startfrom;
    vector result = newvector(numberofelements ,original.typesize);
    for(curindex = 0; curindex < numberofelements; curindex++){
        setelement(result, curindex, getelementpointer(original,startfrom + curindex));
    }
    return original;
}

unsigned int
vectormemcmp(char * label, vector v,unsigned int vectorstartoffset, char * m, unsigned int bytes){
    cprintf("Checking complience in %s:", label);

    unsigned int byteindex = 0;
    //int identical = 1;
    for(byteindex = 0; byteindex < bytes; byteindex++){
        char * elementpointer = getelementpointer(v,byteindex + vectorstartoffset); 
        if(elementpointer == NULL){
            cprintf("Vector is %d bytes, could not access byte number %d.", byteindex + 1, byteindex + 2);
            cprintf("Vector actual length is: %d.\n", v.vectorsize);
            return 0;
        } else if(m[byteindex] != *getelementpointer(v,byteindex + vectorstartoffset)){
            cprintf("Diff in v[%d]=%d vs. m[%d]=%d. offset=%d\n",byteindex + vectorstartoffset,*getelementpointer(v,byteindex + vectorstartoffset),byteindex,m[byteindex],vectorstartoffset);
            return 0; 
        }
    }
    cprintf("success.\n");
    return 1; // = identical
}

unsigned int
copysubvector(vector * dstvector, vector * srcvector, unsigned int srcoffset, unsigned int count){
    check_existence(dstvector, KVEC_ERR);
    check_existence(srcvector, KVEC_ERR);
    check_validity(dstvector, KVEC_ERR);
    check_validity(srcvector, KVEC_ERR);
    if(dstvector->vectorsize < count)
        return KVEC_ERR;

    unsigned int curindex;
    for(curindex = 0; curindex < count; curindex++){
        setelement(*dstvector, curindex, getelementpointer(*srcvector,srcoffset + curindex));
    }
    dstvector->vectorsize = count;
    return 1; // SUCCESS
}