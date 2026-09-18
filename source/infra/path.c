#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../shared/logger.h"

int path_is_file(const char *path)
{
	// 1. Инициализируем структуру статической информации
	struct stat fs = {0};

	// 2. Получаем статическую информацию
	if(stat(path, &fs) != 0) return 0;

	// 3. Определение
	if(S_ISREG(fs.st_mode) == 0) return 0;

	return 1;
}

int path_is_dir(const char *path)
{
	// 1. Инициализируем структуру статической информации
	struct stat fs = {0};

	// 2. Получаем статическую информацию
	if(stat(path, &fs) != 0) return 0;

	// 3. Определение
	if(S_ISDIR(fs.st_mode) == 0) return 0;

	return 1;
}