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
	// 1. Инициализиурем структуру ответа
	HttpResponse response = {0};

	/* Базовые настройки */
	response.version = HTTP_VERSION_1_1;
	response.status_code = status_code;
	response.body = (char *) http_code_to_string(status_code);
	response.body_len = strlen(response.body);

	/* Переводим размер тела в строку */
	char body_size[sizeof(size_t)];
	snprintf(body_size, sizeof(size_t), "%ld", response.body_len);

	// 2. Заполняем заголовки

	/* Инициализация хэш-таблицы */
	response.headers = hashmap_init(10);

	if(response.headers == NULL) return fail;

	/* Тип контента, тип соединения, размер файла */
	if(hashmap_add(&response.headers, "Content-Type","text/plain") == fail
	|| hashmap_add(&response.headers, "Connection","close") == fail
	|| hashmap_add(&response.headers, "Content-Length", body_size) == fail) {
		free_hashmap(response.headers);
		return fail;
	}

	// 3. Переводим структуру в строковый формат
	char headers_buffer[MAX_SIZE_HEADERS];

	if(http_serialize_response(headers_buffer, sizeof(headers_buffer), response) == fail) {
		free_hashmap(response.headers);
		return fail;
	}

	/* Освобождаем таблицу заголовков */
	free_hashmap(response.headers);

	// 4. Отправляем ответ
	ssize_t n = send(client_fd, headers_buffer, strlen(headers_buffer), 0);

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