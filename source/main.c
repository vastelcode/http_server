#include <stdio.h>

#include "shared/prototypes.h"
#include "shared/types.h"
#include "shared/constants.h"

int main(void)
{
	FILE *file = fopen("server.log","a");

	if(file == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось открыть файл",__func__);
		return 1;
	}

	logger(INFO, stdout, NULL, "%s: Успешный запуск программы",__func__);

	logger(INFO, file, NULL, "%s: Запись в файл прошла успешно",__func__);

	fclose(file);
	return 0;
}