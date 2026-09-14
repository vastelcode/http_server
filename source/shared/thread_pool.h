// Public API - infra/thread_pool.c

#ifndef THEAD_POOL

#include "types.h"
#include "constants.h"

#define THREAD_POOL

status_exec thread_pool_start(Context *context, int amount_threads);
status_exec thread_pool_preprocessing(task_t *task);
void thread_pool_submit(Context *context, task_t *task);
void *thread_pool_routine(void *arg);
status_exec thread_pool_stop(Context *context);

#endif