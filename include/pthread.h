#pragma once

/*
 * Our minimal version of pthreads
 */

#include "atomic.hh"

typedef int pthread_t;
typedef int pthread_attr_t;
typedef int pthread_key_t;
typedef int pthread_barrierattr_t;
typedef std::atomic<unsigned> pthread_barrier_t;

int       pthread_create(pthread_t* tid, const pthread_attr_t* attr,
                         void* (*start)(void*), void* arg);
pthread_t pthread_self(void);

int       pthread_key_create(pthread_key_t* key, void (*destructor)(void*));
void*     pthread_getspecific(pthread_key_t key);
int       pthread_setspecific(pthread_key_t key, void* value);

int       pthread_barrier_init(pthread_barrier_t *b,
                               const pthread_barrierattr_t *attr,
                               unsigned count);
int       pthread_barrier_wait(pthread_barrier_t *b);
