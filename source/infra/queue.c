// Модуль для работы с очередью

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../shared/queue.h"
#include "../shared/logger.h"
#include "../shared/types.h"
#include "../shared/constants.h"

void free_task(task_t *task)
{
	if(task) {
		if(task->request) free(task->request);
		free(task);
	}
}

void free_queue(Queue *queue)
{
	for(size_t i = 0; queue->data[i] != NULL; i++) free_task(queue->data[i]);
	free(queue->data);
	free(queue);
}

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

void queue_push(Queue *queue, task_t *task)
{
	if(queue->amount == queue->capacity) {
		logger(WARN, stdout, NULL, "%s: Очередь переполнена", __func__);
		return;
	}
	
	queue->data[queue->amount++] = task; // увеличиваем счётчик элементов и добавляем новый элемент в конец массива
}

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