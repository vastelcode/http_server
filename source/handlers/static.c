#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include "../shared/logger.h"
#include "../shared/constants.h"
#include "../shared/path.h"
#include "../shared/http.h"
#include "../shared/hashmap.h"
#include "../shared/dstr.h"
#include "../shared/server.h"
#include "../shared/types.h"

status_exec static_handler(task_t *task, HashMap *config)
{
	if(config == NULL) return fail; /* заглушка */

	// 1. Проверяем права на чтение

	// 1.1 Получаем статические данные
	struct stat fs = {0};
	if(stat(task->fullpath, &fs) != 0) {
		logger(ERROR, stdout, NULL, "%s: Не удалось получить статические данные о файле (%s)", __func__, task->fullpath);
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	off_t size = fs.st_size;

	// 1.2 Проверка прав
	if(!(fs.st_mode & S_IROTH)) {
		logger(ERROR, stdout, NULL, "%s: Нет прав на чтение файла (%s) ",__func__, task->fullpath);
		http_send_error(task->client_fd, ForBidden);
		return fail;
	}

	// 2. Получаем MIME-тип 
	HashMap *table = path_init_mime();

	char *mime_type = path_get_mime(table, task->fullpath);
	
	free_hashmap(table);

	if(mime_type == NULL) {
		http_send_error(task->client_fd, BadRequest);
		return fail;
	}

	// 3. Формируем заголовки

	// 3.1 Инициализируем структуру ответа
	HttpResponse response = {0};

	/* Базовые настройки */
	response.version = HTTP_VERSION_1_1;
	response.status_code = OK;
	response.body_len = (size_t) size;

	/* Переводим размер тела в строку*/
	char body_size[20];
	snprintf(body_size, sizeof(size_t), "%ld", response.body_len);

	// 3.2 Запись заголовков

	/* Заголовки */
	response.headers = hashmap_init(default_hashmap_capacity);

	if(!response.headers) {
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	/* Тип контента, тип соединения, размер файла */
	if(hashmap_add(&response.headers, "Content-Type",mime_type) == fail
	|| hashmap_add(&response.headers, "Connection","close") == fail
	|| hashmap_add(&response.headers, "Content-Length", body_size) == fail) {
		free_hashmap(response.headers);
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	/* Записываем заголовки и статусную строку в буфер */
	char headers_buffer[MAX_SIZE_HEADERS];

	if(http_serialize_response(headers_buffer, sizeof(headers_buffer), response) == fail) {
		free_hashmap(response.headers);
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	/* Освобождаем таблицу заголовков */
	free_hashmap(response.headers);

	// 3.3 Отправка заголовков клиенту

	if(socket_send_all(task->client_fd, headers_buffer, strlen(headers_buffer)) == -1) {
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	// 4. Открываем файл на чтение
	int fd = open(task->fullpath, R_OK);

	if(fd == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось открыть файл на чтение (%s)",__func__, task->fullpath);
		http_send_error(task->client_fd, IntervalServerError);
		return fail;
	}

	// 5. Отправляем содержимое файла клиенту

	ssize_t sent = 0; /* кол-во отправленных байт */

	/* пока не отправим весь файл */
	while(sent < size) {
		char buffer[BATCH_SIZE]; /* буфер для отправки */
		ssize_t readn = read(fd, buffer, sizeof(buffer)); /* читаем порцию данных из файла */

		if(readn == -1) {
			logger(ERROR, stdout, NULL, "%s: Не удалось прочитать порцию данных (%s)",__func__, task->fullpath);
			http_send_error(task->client_fd, IntervalServerError);
			close(fd);
			return fail;
		}

		buffer[readn] = '\0'; /* завершаем строку */

		ssize_t sendn = send(task->client_fd, buffer, strlen(buffer), 0); /* отправляем порцию данных клиенту */

		if(sendn == -1) {
			logger(ERROR, stdout, NULL, "%s: Не удалось отправить порцию данных (%s)",__func__, task->fullpath);
			http_send_error(task->client_fd, IntervalServerError);
			close(fd);
			return fail;
		}

		if(sendn == 0) {
			logger(INFO, stdout, NULL, "%s: Клиент закрыл соединение",__func__);
			break;
		}

		sent += sendn; /* добавляем кол-во отправленных байт */
	}

	/* Отправляем сообщение об успехе */
	logger(INFO, stdout, NULL, "%d (%s) %s %s", OK, "OK", "GET", task->request->path);

	/* Закрываем дескрипторы */
	close(fd);
	close(task->client_fd);

	return success;
}