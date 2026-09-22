// Public API - infra/thread_pool.c

#ifndef THEAD_POOL

#include "types.h"
#include "constants.h"

#define THREAD_POOL

/**
 * @brief Создаёт опреденённое количество потоков, каждый из которых будет ожидать добавления задачи в очередь
 * 
 * @param[in]  amount_threads Кол-во создаваемых потоков
 * @param[out]  context  Указатель на структуру контекста пула потоков
 *
 *
 * @note Параметр context должен быть определён заранее
 */
status_exec thread_pool_start(Context *context, int amount_threads);
void thread_pool_submit(Context *context, task_t *task);
status_exec thread_pool_stop(Context *context);

#endif