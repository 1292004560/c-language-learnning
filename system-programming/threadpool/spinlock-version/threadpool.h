#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

typedef struct thread_pool_s thread_pool_t;

typedef void (*handler_t)(void *);

thread_pool_t *thread_pool_create(int thread_count);

void thread_pool_terminate(thread_pool_t *pool);

int thread_pool_post(thread_pool_t *pool, handler_t handler, void *arg);

void thread_pool_waitdone(thread_pool_t *pool);

#endif
