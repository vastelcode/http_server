#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/dstr.h"
#include "../shared/http.h"
#include "../shared/types.h"
#include "../shared/logger.h"

status_exec http_serialize_response(char *buf, size_t buffsize, const HttpResponse res)
{
	// 1. Инициализация динамической строки
	dstr_t response = {0};
	if(dstr_init(&response, buffsize) == -1) return fail;

	// 2. Запись статусной строки
	if(dstr_appendf(&response, "%s %d %s\r\n", http_version_to_string(res.version), res.status_code, http_code_to_string(res.status_code)) == -1) {
		free((&response)->buffer);
		return fail;
	}

	// 3. Запись заголовков
	for(size_t i = 0; i < res.headers->capacity; i++) {
		node_t *node = res.headers->data[i];
		
		while(node) {
			if(dstr_appendf(&response, "%s: %s\r\n", node->key, node->value) == -1) {
				free((&response)->buffer);
				return fail;
			}

			node = node->next;
		}
	}

	/* Завершаем блок с заголовками */
	if(dstr_appendf(&response, "\r\n") == -1) {
		free((&response)->buffer);
		return fail;
	}

	// 4. Запись тела
	if(res.body && res.body_len > 0) {
		if(dstr_appendf(&response, "%s\r\n", res.body) == -1) {
			free((&response)->buffer);
			return fail;
		}
	}

	// 5. Запись полученной строки в буфер
	strncpy(buf, response.buffer, buffsize);
	free((&response)->buffer);
	return success;
}