// Модуль для работы с конфигурационным файлом

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../shared/types.h"
#include "../shared/constants.h"
#include "../shared/prototypes.h"

#define CONFIG_PATH "server.conf" // путь до конфигурационного файла

char *standart_config[] = {"port=8080","threads=4","logfile=server.log","root=www","default_file=index.html","cgi_dir=cgi-bin","backlog=5",NULL}; // стандартные настройки (минимально необходимые для работы сервера)

int extract_param_long(HashMap *config, char *key, int default_val, int min_val, int max_val)
{	
	node_t *node_key = get_node_hashmap(config,key);

	long value;
	if(node_key) value = extract_long(node_key->value);
	else value = default_val;

	if(value == fail || value < min_val || value > max_val) {
		logger(WARN,stdout, NULL, "Параметр %s принимает значение по умолчанию", key);
		value = default_val;
	}

	return (int) value;
}

status_exec add_parameter(HashMap **hashmap,char *buffer)
{
	char **tokens = split_by_subs(buffer,"=", 3); // делим строку на токены

	if(tokens == NULL) return fail;

	// подсчитываем кол-во токенов
	size_t amount_tokens = 0;
	for(;tokens[amount_tokens] != NULL; amount_tokens++);

	if(amount_tokens != 2) {
		logger(WARN, stdout, NULL, "%s: Обнаружена неправильная строка в конфигурации - \"%s\"",__func__, buffer);
		free_to_count((void **) tokens, amount_tokens);
		return warning;
	}

	// получаем ключ и значение
	char *key = tokens[0];
	char *value = tokens[1];

	status_exec status_add = add_hashmap(hashmap, key, value);	 // добавляем пару ключ-значение в хэш-таблицу
	
	if(status_add == fail) {
		free_to_count((void **) tokens, 2);
		return fail;
	}

	free_to_count((void **) tokens, 2);

	return success;
}

HashMap *get_default_config(void)
{
	HashMap *hashmap = init_hashmap(default_hashmap_capacity); // выделяем память на хэш-таблицу

	if(hashmap == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
		return NULL;
	}

	for(size_t i = 0; standart_config[i] != NULL; i++) {

		if(add_parameter(&hashmap, standart_config[i]) == fail) {
			free_hashmap(hashmap);
			return NULL;
		}
		
	}

	return hashmap;
}

HashMap *get_config(void)
{
	// 1. Получаем настройки по умолчанию
	HashMap *config = get_default_config();

	if(config == NULL) return NULL;

	// 2. Формируем путь до конфигурационного файла

	// 2.1 Получаем рабочую директорию
	char cwd[1024];

	if(getcwd(cwd, sizeof(cwd)) == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось получить рабочую директорию",__func__);
		free_hashmap(config);
		return NULL;
	}

	// 2.2 Формируем полный путь

	dstr_t path = {0};
	if(dstr_init(&path, base_length_dstr) == -1) {
		free_hashmap(config);
		return NULL;
	}
	if(dstr_multi_append(&path, 3, cwd, "/", CONFIG_PATH) == -1) {
		free_hashmap(config);
		free((&path)->buffer);
		return NULL;
	}

	char *config_path = (&path)->buffer;

	FILE *file = fopen(config_path,"r"); // открываем файл на чтение

	if(file == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось открыть файл \"%s\"",__func__, config_path);
		free(config_path);
		free_hashmap(config);
		return NULL;
	}

	free(config_path);

	char buffer[default_size_buffer_string]; // буфер для записи строки

	// построчно читаем файл
	while(fgets(buffer, sizeof(buffer), file)) {

		buffer[strcspn(buffer, "\n")] = '\0'; // заменяем символ перевода строки на нулевой

		// добавляем строку конфигурации в хэш-таблицу
		if(add_parameter(&config, buffer) == fail) {
			free_hashmap(config);
			fclose(file);
			return NULL;
		}
	}

	fclose(file);

	return config;
}