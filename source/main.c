#include <stdio.h>
#include <stdlib.h>

#include "shared/prototypes.h"
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

	// 3. Инициализация очереди задач
	
	// 3.1 Извлечение количества потоков из конфигурации
	int amount_threads = extract_param_long(config, "threads", default_amount_threads, min_amount_threads, max_amount_threads);

	// 3.2 Выделение памяти на очередь
	Queue *tasks = queue_init(amount_threads * 4);

	if(tasks == NULL) {
		free_hashmap(config);
		return 3;
	}

	logger(INFO, stdout, NULL, "%s: Успешная инициализация очереди",__func__);

	free_hashmap(config);
	free_queue(tasks);

	return 0;
}