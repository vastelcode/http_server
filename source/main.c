#include <stdio.h>

#include "shared/prototypes.h"
#include "shared/types.h"
#include "shared/constants.h"

int main(void)
{
	// 1. Чтение конфигурационного файла
	HashMap *config = get_config();
	if(config == NULL) return 1;
	logger(INFO, stdout, NULL, "%s: Успешное чтение конфигурации",__func__);

	// 2. Инициализация очереди задач

	// 2.1 Извлечение количества потоков из конфигурации
	int amount_threads = extract_param_long(config, "threads", default_amount_threads, min_amount_threads, max_amount_threads);

	// 2.2 Выделение памяти на очередь
	Queue *tasks = queue_init(amount_threads * 4);

	if(tasks == NULL) {
		free_hashmap(config);
		return 2;
	}

	logger(INFO, stdout, NULL, "%s: Успешная инициализация очереди",__func__);

	free_hashmap(config);
	free_queue(tasks);

	return 0;
}