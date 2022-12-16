#include "unittests.h"
#include "../../kvector.h"
#include <stdio.h>
#include <string.h>

#define isInfoEqual(info1, info2) ((info1).age == (info2).age && (info1).height == (info2).height && strcmp((info1).name,(info2).name) == 0)

//external functions
void initTestingEnvironment();

// API BEING TESTED
vector newvector(unsigned int size, unsigned int typesize);
void freevector(vector * v);
void memmove_into_vector_bytes(vector dstvec,unsigned int dstbyteoffset, char* src, unsigned int size);
void memmove_into_vector_elements(vector dstvec, unsigned int dstelementoffset, char* src, unsigned int size);
void memmove_from_vector(char * dst, vector vec, unsigned int elementoffset, unsigned int elementcount);
char* getelementpointer(vector v, unsigned int index);


int initializeVectorTest(){
    char testName[] = "initializeVectorTest";
    int vectorsize;
    int typesize;
    int passedTest = 1;
    vector v;

    for(vectorsize = 1; vectorsize < 1000 ; vectorsize += 100){
        for(typesize = 1; typesize < 100 ; typesize += 20){
            // create new vector
            v = newvector(vectorsize, typesize);
            // check the success of the initialization
            if(v.valid == 0){
                //vector is not valid
                FAILTEST(testName);
                printf("Failed to initialize vector with size = %d and typesize = %d.\n", vectorsize, typesize);
                freevector(&v);
                return FAIL;
            }
        }
    }
    PASSTEST(testName);
    freevector(&v);
    return PASS;
}

int readAndWriteDataTest(){
    char testName[] = "readAndWriteDataTest";

    // Some composite data
    struct info {
        int age;
        float height ;
        char name[10];
    };

    struct info students[5] = {
        {.age = 17,
        .height = 1.7,
        .name = "Daniel"},
        {.age = 18,
        .height = 1.8,
        .name = "Mike"},
        {.age = 27,
        .height = 2.0,
        .name = "Jordan"},
        {.age = 48,
        .height = 1.6,
        .name = "Elinor"},
        {.age = 32,
        .height = 1.85,
        .name = "Michael"}
    };

    vector composite[] = {  newvector(5, (sizeof(struct info))), 
                            newvector(5, (sizeof(struct info)))};

    memmove_into_vector_bytes(composite[0], 0, (char*)&(students[0]), 5*(sizeof(struct info)));
    memmove_into_vector_elements(composite[1], 0, (char*)&(students[0]), 5);

    int currentVector;
    for(currentVector = 0; currentVector < 2; currentVector++) {
        int currentElement;
        for(currentElement = 1; currentElement < 5; currentElement++){
            struct info currentStudent;
            memmove_from_vector((char*)(&currentStudent), composite[currentVector], currentElement, 1);
            if(!(isInfoEqual(currentStudent, students[currentElement]))){
                FAILTEST(testName);
                printf("Expected Equal values: (%d ?= %d ), (%f ?= %f), (%s ?= %s).\n", currentStudent.age, students[currentElement].age, currentStudent.height, students[currentElement].height, currentStudent.name, students[currentElement].name);
                return FAIL;
            }
        }
    }
    PASSTEST(testName);
    return PASS;
}

int moveBytesTest(){
    char testName[] = "moveBytesTest";

    vector v = newvector(3,1);
    char str[] = "abc";
    memmove_into_vector_bytes(v, 0, str, 3);
    char str2[] = {0,0,0,0};
    memmove_from_vector(str2,v,0,3);

    int result1 = (strcmp(str, str2) == 0);
    if(result1==1){
        PASSTEST(testName);
        return PASS;
    }else{
        FAILTEST(testName);
        return FAIL;
    }
}

int moveBytesWithOffsetTest(){
    char testName[] = "moveBytesWithOffsetTest";

    vector v = newvector(3,1);
    char str[] = "abc";
    memmove_into_vector_bytes(v, 1, str, 2); //"ab" --> vector[empty, a, b]
    char str2[] = {0,0,0,0};                 //str2 is empty string
    memmove_from_vector(str2,v,1,2);         //str2 = {'a','b',0}

    int result1 = (strcmp("ab", str2) == 0);
    if(result1==1){
        PASSTEST(testName);
        return PASS;
    }else{
        FAILTEST(testName);
        return FAIL;
    }
}

int moveElementsTest(){
    char testName[] = "moveElementsTest";

    vector v = newvector(3,sizeof(int));
    int numbers[] = {10, 20, 40};
    memmove_into_vector_elements(v, 0, (char*)numbers, 3);
    

    int result = 1;
    for(int i=0;i<3;i++){
        result = result && (*(int*)getelementpointer(v,i) == numbers[i]);
    }    
    if(result){
        PASSTEST(testName);
        return PASS;
    }else{
        FAILTEST(testName);
        return FAIL;
    }
}

int moveElementsWithOffsetTest(){
    char testName[] = "moveElementsWithOffsetTest";

    vector v = newvector(3,sizeof(int));
    int numbers[] = {10, 20, 40};
    memmove_into_vector_elements(v, 1, (char*)numbers, 2);
    

    int result = 1;
    for(int i=1;i<3;i++){
        result = result && (*(int*)getelementpointer(v,i) == numbers[i-1]);
    }    
    if(result){
        PASSTEST(testName);
        return PASS;
    }else{
        FAILTEST(testName);
        return FAIL;
    }
}

int main(){
    //Initialize mock
    initTestingEnvironment();

    //Define custom messages for success or failure
    char PASS_OR_FAIL_MESSAGES[2][50] = {
        "AT LEAST ONE TEST HAS FAILED.",
        "ALL TESTS HAVE PASSED."
    };

    //Run all tests and save the overall success as a result
    int testResult =    initializeVectorTest() &&
                        moveBytesWithOffsetTest() &&
                        moveBytesTest() &&
                        moveElementsTest() &&
                        moveElementsWithOffsetTest() &&
                        readAndWriteDataTest();

    //Print message
    printf("TEST STATUS: %s\n", PASS_OR_FAIL_MESSAGES[testResult]);

    return !testResult;
}