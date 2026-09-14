// Модуль утилитарных функций

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared/logger.h"
#include "shared/free.h"
#include "shared/types.h"
#include "shared/constants.h"

char **split_by_subs(char *buffer, const char *delim, size_t max_amount)
{
	char **tokens = calloc(max_amount + 1, sizeof(char *)); // выделяем память на массив токенов

	if(!tokens) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
		return NULL;
	}

	//  определяем указатель на начало и конец буфера
	char *current = buffer;
	char *end = &(buffer[strlen(buffer)]);

	// инициализируем счётчик токенов
	size_t cnt_tokens = 0;

	while(cnt_tokens < max_amount) {

		char *pos_delim = strstr(current, delim); // получаем позицию вхождения подстроки

		char *end_ptr = (pos_delim) ? pos_delim : end; // определяем конечную позицию

		int length = end_ptr - current; // определяем длину строки

		tokens[cnt_tokens] = malloc(sizeof(char) * (length + 1)); // выделяем память на строку

		if(tokens[cnt_tokens] == NULL) {
			logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
			free_to_null((void **) tokens);
			return NULL;
		}

		size_t cnt_sym = 0; // инициализируем счётчик символов

		// записываем символы в токен до конечной позиции
		while(current != end_ptr) {
			tokens[cnt_tokens][cnt_sym++] = *current;
			current++;
		}

		tokens[cnt_tokens++][cnt_sym] = '\0'; // завершаем токен

		if(current != end) current += strlen(delim);
		else break;
	}

	return tokens;
}

long extract_long(char *string)
{
	char *endptr; // определяем вспомогательный указатель

	long number = strtol(string, &endptr, 10); // извлекаем число из строки

	if(endptr == string) {
		logger(ERROR, stdout, NULL, "%s: Не удалось извлечь число из строки",__func__);
		return fail;
	}

	return number;
}

size_t count_to_null(void **array)
{
	size_t amount = 0;
	for(;array[amount] != NULL; amount++);
	return amount;
}
