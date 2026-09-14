// Public API - infra/queue.c
#ifndef QUEUE

#include <stdio.h>
#include "types.h"

#define QUEUE


/**
 * @brief Освобождение задачи
 *
 * @param[in, out]  task  Указатель на структуру задачи, подлежащую освобождению
 *
 * @note Если task не NULL, освобождается память, занятая полем request,
 *       а затем и сама структура.
 */
void free_task(task_t *task);

/**
 * @brief Освобождение очереди
 *
 * @param[in,out]  queue  Указатель на структуру очереди, подлежащую освобождению
 *
 * @note Освобождаются все задачи, находящиеся в очереди, массив данных
 *       queue->data и сама структура queue.
 * @see free_task
 */
void free_queue(Queue *queue);

/**
 * @brief Инициализация очереди
 *
 * @param[in]  capacity  Начальное количество ячеек в массиве
 *
 * @return
 * - Указатель на структуру Queue в случае успеха
 * @return
 * - NULL в случае ошибки
 *
 * @note Выделяется память под структуру Queue и массив данных. Поля
 *       capacity и amount инициализируются соответствующими значениями.
 * @see free_queue
 */
Queue *queue_init(size_t capacity);

/**
 * @brief Добавление задачи в очередь
 *
 * @param[in,out]  queue  Указатель на структуру очереди
 * @param[in]      task   Указатель на задачу, добавляемую в очередь
 *
 * @note Если очередь заполнена (amount == capacity), выводится предупреждение
 *       и добавление не производится. В противном случае задача добавляется
 *       в конец массива, а счётчик amount увеличивается.
 * @see queue_shift
 */
void queue_push(Queue *queue,task_t *task);

/**
 * @brief Извлечение задачи из очереди
 *
 * @param[in,out]  queue    Указатель на структуру очереди
 * @param[in]      releace  Флаг освобождения памяти (0 - не освобождать, 1 - освободить)
 *
 * @return
 * - Указатель на извлечённую задачу task_t, если releace == 0
 * @return
 * - NULL, если releace != 0 или очередь пуста
 *
 * @note Если очередь пуста, выводится предупреждение и возвращается NULL.
 *       В противном случае сохраняется указатель на первый элемент, все
 *       последующие элементы сдвигаются на одну позицию влево, счётчик
 *       amount уменьшается. Если установлен флаг releace, извлечённая
 *       задача освобождается через free_task.
 * @see queue_push
 * @see free_task
 */
task_t *queue_shift(Queue *queue, int releace);

#endif