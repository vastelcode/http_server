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

	log_hashmap(config);

	free_hashmap(config);
	return 0;
}