#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include <stdio.h> 

#define FAILTEST(str) printf("Test has failed: %s.\n", str)
#define PASSTEST(str) printf("Test has passed: %s.\n", str)
#define PASSALL(msg) printf("%s.\n", msg)
#define FAILALL(msg) printf("%s.\n", msg)

#define FAIL 0
#define PASS 1

#endif