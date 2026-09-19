#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>
#include "../shared/logger.h"
#include "../shared/hashmap.h"
#include "../shared/queue.h"
#include "../shared/config.h"
#include "../shared/thread_pool.h"
#include "../shared/free.h"
#include "../shared/server.h"
#include "../shared/path.h"
#include "../shared/types.h"
#include "../shared/constants.h"

// инициализируем значения глобальных переменных
int pipe_fd[2]; // массив файловых дескрипторов
int running = 1; // флаг запуска сервера

static void handle_sigint(int sig)
{
	if(sig == SIGINT) {
		running = 0;
		write(pipe_fd[1],"0",1);
	}
}

status_exec server_run(HashMap *config)
{
	// 1. Подготовка

	// 1.1 Установка обработчика сигнала
	signal(SIGINT, handle_sigint);

	// 1.2 Создание pipe
	pipe(pipe_fd);

	// 1.3 Инициализация структуры отслеживаемых событий
	struct pollfd fds[2];
	fds[0].events = POLLIN;
	fds[1].fd = pipe_fd[0];
	fds[1].events = POLLIN;

	// 2. Инициализация очереди задач

	// 2.1 Извлечение количества потоков из конфигурации
	int amount_threads = extract_param_long(config, "threads", default_amount_threads, min_amount_threads, max_amount_threads);

	// 2.2 Выделение памяти на очередь
	Queue *tasks = queue_init(amount_threads * 4);

	if(tasks == NULL) {
		free_hashmap(config);
		return fail;
	}

	logger(INFO, stdout, NULL, "%s: Успешная инициализация очереди",__func__);

	// 3. Инициализация таблицы MIME-типов

	HashMap *mime_table = path_init_mime();

	if(mime_table == NULL) {
		free_hashmap(config);
		free_queue(tasks);
		return fail;
	}

	logger(INFO, stdout, NULL, "%s: Успешная инициализация таблицы MIME-типов",__func__);

	// 4. Запуск пула потоков

	// 4.1 Формирование контекста
	Context ctx = {0};
	ctx.tasks = tasks;
	ctx.config = config;
	ctx.mime_table = mime_table;

	// 4.2 Запуск
	if(thread_pool_start(&ctx, amount_threads) == fail) {
		logger(ERROR, stdout, NULL, "%s: Не удалось запустить потоки выполнения",__func__);
		free_hashmap(config);
		free_queue(tasks);
		free_hashmap(mime_table);
		return fail;
	}

	// 4. Открытие серверного сокета

	// 4.1 Извлекаем параметры backlog и port из конфигурации
	int backlog = extract_param_long(config, "backlog", default_backlog,min_backlog, max_backlog);
	int port = extract_param_long(config, "port", default_port, min_port, max_port);

	// 4.2 Инициализируем структуру сетевого адреса
	struct sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons((uint16_t) port);
	socklen_t addrlen = sizeof(addr);

	// 4.3 Открываем серверный сокет
	int server = socket_create((struct sockaddr *) &addr, backlog);

	if(server == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось запустить сервер", __func__);
		thread_pool_stop(&ctx);
		return fail;
	}

	logger(INFO, stdout, NULL, "%s: Сервер запущен на порту %d...",__func__, port);
	
	fds[0].fd = server; // добавляем в структуру отслеживаемых событий серверный дескриптор

	// 5. Запуск приёма соединений

	while(running) {
		int ret = poll(fds,2,-1); // мониторим файловые дескрипторы на события

		if(ret < 0) {
			if(errno == EINTR) continue;
			logger(ERROR, stdout, NULL, "%s: Не удалось начать наблюдение",__func__);
			break;
		}

		// если проснулись из-за сигнала, то завершаем работу цикла
		if (fds[1].revents & POLLIN) {
            // Очищаем pipe
            char buf[8];
            read(pipe_fd[0], buf, sizeof(buf));
            break;
        }

		if(fds[0].revents & POLLIN) {

			int client = accept(server,(struct sockaddr *) &addr, &addrlen); // получаем клиентский сокет

			if(client == -1) {
				if(errno == EINTR) continue;
				logger(ERROR, stdout, NULL, "%s: Не удалось принять соединение",__func__);
				break;
			}

			// создаём новую задачу
			task_t *new_task = calloc(1,sizeof(task_t));

			if(!new_task) {
				logger(ERROR, stdout, NULL, "%s: Не удалось создать",__func__);
				close(client);
				break;
			}

			new_task->client_fd = client; // передаём файловый дескриптор

			thread_pool_submit(&ctx, new_task); // отправляем задачу в очередь
		}
	}
	
	thread_pool_stop(&ctx); // останавливаем работу потоков
	close(server); // закрываем серверный дескриптор

	logger(INFO, stdout, NULL, "%s: Работа сервера завершена",__func__);
	return success;
}