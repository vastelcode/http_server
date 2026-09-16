#include <stdio.h>
#include "shared/logger.h"
#include "shared/hashmap.h"
#include "shared/config.h"
#include "shared/core.h"
#include "shared/types.h"
#include "shared/constants.h"


int main(void)
{
	// 1. Чтение конфигурационного файла
	HashMap *config = get_config();
	if(config == NULL) return 1;
	logger(INFO, stdout, NULL, "%s: Успешное чтение конфигурации",__func__);

	// 2. Перенаправление выходного потока
	
	// 2.1 Извлечение целевого узла из хэш-таблицы
	node_t *node_logfile = hashmap_get_node(config, "logfile");

	if(node_logfile == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось извлечь параметр конфигурации \"%s\"",__func__,"logfile");
		free_hashmap(config);
		return 2;
	}

	logger(INFO, stdout, NULL, "%s: Перенаправление потока вывода в файл \"%s\"",__func__,node_logfile->value);

	// 2.2 Очистка файла
	FILE *logfile = fopen(node_logfile->value,"w");

	if(logfile == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось открыть файл \"%s\"",__func__, node_logfile->value);
		free_hashmap(config);
		return 2;
	}

	fclose(logfile);

	// 2.3 Перенаправление stdout
	if(freopen(node_logfile->value, "a", stdout) == NULL) {
		logger(WARN, stdout, NULL, "%s: Не удалось перенаправить поток вывода в файл \"%s\"",__func__,node_logfile->value);
	}

	// 3. Запускаем сервер
	if(server_run(config) == fail) return 3;

	return 0;
}