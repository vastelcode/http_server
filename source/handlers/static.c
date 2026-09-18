#include <stdio.h>
#include <unistd.h>
#include "../shared/logger.h"
#include "../shared/types.h"

status_exec static_handler(task_t *task, HashMap *config)
{
	if(config == NULL) return fail;
	printf("Static: Полный путь = %s | Путь запроса = %s\n",task->fullpath, task->request->path);
	close(task->client_fd);
	return success;
}