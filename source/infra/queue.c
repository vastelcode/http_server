// Модуль для работы с очередью

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../shared/prototypes.h"
#include "../shared/types.h"
#include "../shared/constants.h"

/**
 * @brief Освобождение задачи
 *
 * @param[in, out]  task  Указатель на структуру задачи, подлежащую освобождению
 *
 * @note Если task не NULL, освобождается память, занятая полем request,
 *       а затем и сама структура.
 */
void free_task(task_t *task)
{
	if(task) {
		if(task->request) free(task->request);
		free(task);
	}
}

/**
 * @brief Освобождение очереди
 *
 * @param[in,out]  queue  Указатель на структуру очереди, подлежащую освобождению
 *
 * @note Освобождаются все задачи, находящиеся в очереди, массив данных
 *       queue->data и сама структура queue.
 * @see free_task
 */
void free_queue(Queue *queue)
{
	for(size_t i = 0; queue->data[i] != NULL; i++) free_task(queue->data[i]);
	free(queue->data);
	free(queue);
}

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
Queue *queue_init(size_t capacity)
{
	Queue *queue = calloc(1, sizeof(Queue)); // выделяем память на структуру

	if(queue == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
		return NULL;
	}

	queue->data = calloc(capacity + 1, sizeof(task_t *)); // выделяем память на массив с данными

	if(queue->data == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
		free(queue);
		return NULL;
	}

	// инициализируем размерные значения
	queue->capacity = capacity;
	queue->amount = 0;

	return queue;
}

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
void queue_push(Queue *queue, task_t *task)
{
	if(queue->amount == queue->capacity) {
		logger(WARN, stdout, NULL, "%s: Очередь переполнена", __func__);
		return;
	}
	
	queue->data[queue->amount++] = task; // увеличиваем счётчик элементов и добавляем новый элемент в конец массива
}

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
task_t *queue_shift(Queue *queue, int releace)
{
	if(queue->amount < 1) {
		logger(WARN, stdout, NULL, "%s: Невозможно удалить элемент очереди", __func__);
		return NULL;
	}

	task_t *target = queue->data[0]; // сохраняем указатель на первый элемент

	for(size_t i = 0; i < queue->amount - 1; i++) 
		queue->data[i] = queue->data[i + 1]; // осуществляем сдвиг массива

	queue->amount--; // уменьшаем счётчик
	queue->data[queue->amount] = NULL; // обнуляем удалённый элемент

	if(releace) {
		free(target); // освобождаем память
		return NULL;
	}

	return target; // возвращаем указатель на удалённый элемент
}