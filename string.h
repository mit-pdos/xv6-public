#ifndef XV6_STRING_H
#define XV6_STRING_H
#pragma once

#include "types.h"

int    memcmp(const void*, const void*, uint);
void*  memmove(void*, const void*, uint);
void*  memcpy(void *, const void *, uint);
void*  memset(void*, int, uint);
char*  safestrcpy(char*, const char*, int);
int    strlen(const char*);
int    strcmp(const char*, const char*);
int    strncmp(const char*, const char*, uint);
char*  strncpy(char*, const char*, int);

#endif /* XV6_STRING_H */
 