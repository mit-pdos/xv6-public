#ifndef _UNITMOCK_H_
#define _UNITMOCK_H_

#include "../../types.h"
#include "../../mmu.h"
#include <stdio.h>

#define NUMBER_OF_PAGES 10000
#define cprintf(...) printf(__VA_ARGS__)

char memory[NUMBER_OF_PAGES][PGSIZE] = {0};
int availabilityIndex[NUMBER_OF_PAGES];

void initTestingEnvironment(){
    //setup memory
    for(int i=0;i<NUMBER_OF_PAGES;i++){
        availabilityIndex[i] = 1;
    }
}

char * kalloc(){
    for(int i=0;i<NUMBER_OF_PAGES;i++){
        if(availabilityIndex[i] == 1){
            availabilityIndex[i] = 0;
            return memory[i]; 
        }
    }
    return NULL;
}

void kfree(char * ptr){
    for(int i=0;i<NUMBER_OF_PAGES;i++){
        if(ptr == &memory[i][0]){
            availabilityIndex[i] = 0;
        }
    }
}
#endif