#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../shared/logger.h"
#include "../shared/dstr.h"
#include "../shared/http.h"
#include "../shared/utils.h"
#include "../shared/hashmap.h"
#include "../shared/types.h"
#include "../shared/constants.h"

status_exec http_send_error(int client_fd, http_code_t status_code)
{
	// 1. Инициализируем динамическую строку ответа
	dstr_t res;

	if(dstr_init(&res, 10) == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось сформировать ответ клиенту",__func__);
		return fail;
	}

	// 2. Записываем статусную строку

	// 2.1 Получаем статусное сообщение и версию ответа
	const char *message = http_code_to_string(status_code);
	const char *version = http_version_to_string(HTTP_VERSION_1_1);

	// 2.2 Записываем строку
	if(dstr_appendf(&res, "%s %d %s\r\n", version, status_code, message) == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось сформировать ответ клиенту",__func__);
		free((&res)->buffer);
		return fail;
	}

	// 3. Записываем заголовки и тело ответа

	// 3.1 Записываем размер тела в строку
	char body_size[20];
	snprintf(body_size, sizeof(body_size), "%ld", strlen(message));

	// 3.2 Записываем общую форматную строку
	if(dstr_appendf(&res, "%s: %s\r\n%s: %s\r\n%s: %s\r\n\r\n%s", "Content-Type","text/plain","Content-Length",body_size ,"Connection","close", message) == -1) {
		logger(ERROR, stdout, NULL, "%s: Не удалось сформировать ответ клиенту",__func__);
		free((&res)->buffer);
		return fail;
	}

	// 4. Отправляем ответ
	ssize_t n = send(client_fd, res.buffer, res.length, 0);

	free((&res)->buffer);

	if(n < 0) {
		logger(ERROR, stdout, NULL, "%s: Не удалось отправить ответ клиенту",__func__);
		return fail;
	}

	if(n == 0) {
		logger(WARN, stdout, NULL, "%s: Клиент закрыл соединение",__func__);
		return warning;
	}
	
	return success;
}