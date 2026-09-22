#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../shared/hashmap.h"
#include "../shared/constants.h"
#include "../shared/dstr.h"
#include "../shared/http.h"
#include "../shared/utils.h"
#include "../shared/types.h"
#include "../shared/config.h"
#include "../shared/logger.h"

int cgi_check(HttpRequest *req, HashMap *config)
{
	// 1. Проверяем есть ли строка cgi_dir в конфигурации
	node_t *cgi_dir = hashmap_get_node(config, "cgi_dir");
	if(!cgi_dir || is_empty(cgi_dir->value)) return 0;

	// 2. Проверяем метод
	if(req->method != HTTP_METHOD_GET && req->method != HTTP_METHOD_POST) return 0;

	// 3. Проверяем соответствие путей

	// 3.1 Формируем ожидаемый префиксный путь
	dstr_t prefix_path = {0};
	if(dstr_init(&prefix_path, 8) == -1) return 0;
	if(dstr_appendf(&prefix_path, "/%s/", cgi_dir->value) == -1) {
		free((&prefix_path)->buffer);
		return 0;
	}

	// 3.2 Сравниваем первые N-символов
	if(strncmp(req->path, prefix_path.buffer, prefix_path.length) != 0) {
		free((&prefix_path)->buffer);
		return 0;
	}

	// 3.3 Проверяем, что после префикса есть хотя бы один символ
	if(strcmp(req->path, prefix_path.buffer) == 0) {
		free((&prefix_path)->buffer);
		return 0;
	}

	free((&prefix_path)->buffer);

	return 1;
}

status_exec cgi_handler(task_t *task, HashMap *mime_table)
{
	if(mime_table == NULL) return fail; /* заглушка */
	http_send_error(task->client_fd, OK);
	close(task->client_fd);
	return success;
}