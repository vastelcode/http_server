#include "../shared/types.h"
#include "../shared/hashmap.h"
#include "../shared/http.h"
#include "../shared/dstr.h"
#include "../shared/logger.h"
#include "../shared/utils.h"
#include "../shared/cgi_handlers.h"
#include "../shared/static_handlers.h"
#include "../shared/path.h"

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

http_code_t dispatch_request(task_t *task, HashMap *config)
{
	// 1. Валидация метода запроса
	if(task->request->method != HTTP_METHOD_GET) {
		//  Отправляем ошибку "Не поддерживается"
		http_send_error(task->client_fd, NotAllowed);
		return NotAllowed;
	}

	// 2. Определение обработчика запроса

	// 2.1 Определяем является ли путь CGI
	int is_cgi = cgi_check(task->request, config);

	// 2.2 Получаем текущую рабочую директорию
	char cwd[1024];

	if(getcwd(cwd, sizeof(cwd)) == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось определить рабочую директорию",__func__);
		http_send_error(task->client_fd, IntervalServerError);
		return IntervalServerError;
	}

	// 2.3 Получаем корневую директорию
	node_t *root_dir = hashmap_get_node(config, "root");

	if(!root_dir || is_empty(root_dir->value)) {
		logger(ERROR, stdout, NULL, "%s: Не удалось определить корневую директорию",__func__);
		http_send_error(task->client_fd, IntervalServerError);
		return IntervalServerError;
	}

	// 2.4 Формируем полный путь
	dstr_t fullpath = {0};
	
	if(dstr_init(&fullpath, 16) == -1) {
		http_send_error(task->client_fd, IntervalServerError);
		return IntervalServerError;
	}

	if(dstr_appendf(&fullpath, "%s/%s%s", cwd,root_dir->value ,task->request->path) == -1) {
		http_send_error(task->client_fd, IntervalServerError);
		free((&fullpath)->buffer);
		return IntervalServerError;
	}

	// 2.5 Проверка пути

	if(path_is_dir(fullpath.buffer)) {

		// получаем файл по умолчанию
		node_t *default_file = hashmap_get_node(config, "default_file");

		if(!default_file || is_empty(default_file->value)) {
			http_send_error(task->client_fd, IntervalServerError);
			free((&fullpath)->buffer);
			return IntervalServerError;
		}

		// добавляем в путь
		if(dstr_appendf(&fullpath,"/%s",default_file->value) == -1) {
			http_send_error(task->client_fd, IntervalServerError);
			free((&fullpath)->buffer);
			return IntervalServerError;
		}
	}

	if(!path_is_file(fullpath.buffer)) {
		http_send_error(task->client_fd, NotFound);
		free((&fullpath)->buffer);
		return NotFound;
	}

	/* Добавляем вызов realpath для защиты от симлинков */
	if((task->fullpath = realpath(fullpath.buffer, NULL)) == NULL) {
		logger(ERROR,stdout, NULL, "%s: Не удалось канонизировать путь", __func__);
		http_send_error(task->client_fd, IntervalServerError);
		free((&fullpath)->buffer);
		return IntervalServerError;
	}

	/* Формируем строку, с которой должен начинаться путь */
	char begin_with[PATH_MAX];
	snprintf(begin_with, sizeof(begin_with), "%s/%s", cwd, root_dir->value);

	/* Канонизируем путь */
	char real_begin[PATH_MAX];

	if(realpath(begin_with, real_begin) == NULL) {
		logger(ERROR,stdout, NULL, "%s: Не удалось канонизировать путь", __func__);
		http_send_error(task->client_fd, IntervalServerError);
		free((&fullpath)->buffer);
		return IntervalServerError;
	}

	/* Сравниваем начало */
	if(strncmp(task->fullpath, real_begin, strlen(real_begin)) != 0
	|| (task->fullpath[strlen(real_begin)] != '\0' && task->fullpath[strlen(real_begin)] != '/')) {
		http_send_error(task->client_fd, ForBidden);
		free((&fullpath)->buffer);
		return ForBidden;
	}

	free((&fullpath)->buffer);

	/* Определяем обработчик */
	if(is_cgi) task->handler = cgi_handler;
	else task->handler = static_handler;

	return OK;
}