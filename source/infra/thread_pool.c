// Модуль для работы с пулом потоковs
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../shared/types.h"
#include "../shared/constants.h"
#include "../shared/hashmap.h"
#include "../shared/logger.h"
#include "../shared/path.h"
#include "../shared/queue.h"
#include "../shared/http.h"
#include "../shared/core.h"

static pthread_t *th = NULL; // инициализируем массив потоков

// инициализируем элементы синхронизации потоков
static pthread_mutex_t mutexTask;
static pthread_cond_t condTask;
static pthread_mutex_t mutexLog;

status_exec thread_pool_stop(Context *context) {

    // 1. Сигналим потокам о завершении
    context->stop_requested = 1;

    // 2. Будим все потоки, ожидающие на cond
    pthread_mutex_lock(&mutexTask);
    pthread_cond_broadcast(&condTask);
    pthread_mutex_unlock(&mutexTask);

    // 3. Дожидаемся завершения каждого потока
    for (size_t i = 0; i < context->created_threads; i++) {
        if(pthread_join(th[i], NULL) != 0) {
			logger(ERROR, stdout, &mutexLog, "%s: Не удалось корректно завершить работу всех потоков выполнения",__func__);
			// Освобождаем все используемые ресурсы
			free(th);
			free_hashmap(context->config);
			free_queue(context->tasks);
			pthread_mutex_destroy(&mutexTask);
    		pthread_cond_destroy(&condTask);
			return fail;
		}
    }

    // 4. Освобождаем память массива потоков
    free(th);
    th = NULL;

    // 5. Уничтожаем синхронизацию
    pthread_mutex_destroy(&mutexTask);
    pthread_mutex_destroy(&mutexLog);
    pthread_cond_destroy(&condTask);

    // 6. Освобождаем контекст (если нужно)
	free_hashmap(context->config);
	free_queue(context->tasks);

    return success;
}

/* Преодобработка задачи (чтение данных от клиента, парсинг HTTP-запроса, директирование задачи)*/
static status_exec thread_pool_preprocessing(task_t *task, Context *ctx)
{
	// 1. Читаем данные от клиента
	char buffer[1024];

	ssize_t n = recv(task->client_fd, buffer, sizeof(buffer), 0);

	if(n <= 0) {
		logger(ERROR, stdout, &mutexLog, "%s: Произошла ошибка при чтении данных от клиента",__func__);
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	buffer[n] = '\0';

	// 2. Парсим HTTP-запрос
	HttpRequest req = {0};

	if(http_parse_request(buffer, strlen(buffer), &req) == fail) {
		logger(ERROR, stdout, &mutexLog, "%s: Произошла ошибка при парсинге HTTP-запроса",__func__);
		http_send_error(task->client_fd, BadRequest);
		return fail;
	}

	task->request = &req;

	// 3. Декодируем путь
	char decoded_path[2048];

	if(path_url_decode(req.path, decoded_path, 2048) == fail) {
		http_send_error(task->client_fd, BadRequest);
		return fail;
	}

	free((&req)->path);
	req.path = NULL;

	// 4. Нормализуем путь
	char *canonized_path = malloc(sizeof(char) * (strlen(decoded_path) + 1));

	if(canonized_path == NULL) {
		logger(ERROR, stdout, &mutexLog, "%s: Не удалось выделить память",__func__);
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	if(path_realpath(decoded_path, canonized_path, strlen(decoded_path) + 1) == fail) {
		logger(ERROR, stdout, &mutexLog, "%s: Не удалось нормализовать путь",__func__);
		http_send_error(task->client_fd, IntervalServerError);
		free(canonized_path);
		return fail;
	}

	req.path = canonized_path;

	// 5. Помещаем задачу в диспетчер
	http_code_t code_dispatch = dispatch_request(task, ctx->config);

	if(code_dispatch != OK) {
		logger(INFO, stdout, &mutexLog, "%d (%s) %s %s",code_dispatch, http_code_to_string(code_dispatch),http_method_to_string(req.method), req.path);
		return fail;
	}

	return success;
}

void *thread_pool_routine(void *arg)
{
	Context *ctx = (Context *) arg;
	Queue *tasks = ctx->tasks;

	while(1) {
		pthread_mutex_lock(&mutexTask);

		while(tasks->amount == 0 && !ctx->stop_requested) pthread_cond_wait(&condTask, &mutexTask);

		if(tasks->amount == 0 && ctx->stop_requested) {
			pthread_mutex_unlock(&mutexTask);
			break;
		}

		task_t *target = queue_shift(tasks, 0); // извлекаем задачу из очереди

		pthread_mutex_unlock(&mutexTask);

		// выполняем задачу
		if(target) {

			if(target->handler && target->handler(target, ctx->config) == fail) close(target->client_fd);

			free_task(target);
		}
	}

	return NULL;
}

void thread_pool_submit(Context *context, task_t *task)
{
	pthread_mutex_lock(&mutexTask);

	// предобработка задачи
	if(thread_pool_preprocessing(task, context) == fail) {
		pthread_mutex_unlock(&mutexTask);
		free_task(task);
		close(task->client_fd);
		return;
	}

	queue_push(context->tasks, task); // добавляем задачу в конец очереди

	pthread_mutex_unlock(&mutexTask);

	pthread_cond_broadcast(&condTask); // посылаем сигнал
}

status_exec thread_pool_start(Context *context, int amount_threads)
{
	// 1. Создаём потоки и элементы синхронизации
	th = malloc(sizeof(pthread_t) * amount_threads); // выделяем память на массив потоков

	if(th == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память",__func__);
		return fail;
	}

	pthread_mutex_init(&mutexTask,NULL);
	pthread_mutex_init(&mutexLog,NULL);
	pthread_cond_init(&condTask,NULL);

	for(size_t i = 0; i < (size_t) amount_threads; i++) {

		if(pthread_create(&th[i], NULL, &thread_pool_routine, (void *) context) != 0) {
			logger(ERROR, stdout, &mutexLog, "%s: Не удалось создать поток",__func__);
			thread_pool_stop(context);
			return fail;
		}

		context->created_threads++;
	}

	return success;
}