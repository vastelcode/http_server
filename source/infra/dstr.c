// Модуль для работы со строкой динамического размера
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "../shared/logger.h"
#include "../shared/types.h"
#include "../shared/constants.h"

void free_dstr(dstr_t *dstr)
{
	if(dstr) {
		if(dstr->buffer) free(dstr->buffer);
		free(dstr);
	}
}

ssize_t dstr_init(dstr_t *string, size_t capacity)
{
	if(string == NULL) {
		logger(ERROR, stdout, NULL, "%s: Структура имеет неопределённое значение",__func__);
		return -1;
	}

	string->buffer = calloc(capacity + 1, sizeof(char)); // выделяем память на строку

	if(string->buffer == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память",__func__);
		return -1;
	}

	// инициализируем поля структуры
	string->capacity = capacity;
	string->length = 0;

	return 0;
}

ssize_t dstr_append(dstr_t *string, const char *buff)
{
	if(string == NULL || string->buffer == NULL) {
		logger(ERROR, stdout, NULL, "%s: Структура или буфер имеют неопределённое значение",__func__);
		return -1;
	}

	size_t bufflen = strlen(buff); // получаем длину добавляемой строки

	// расширяем строку при необходимости
	if(string->length + bufflen + 1 > string->capacity) {
		char *new_buffer = realloc(string->buffer,sizeof(char) * (string->capacity + (string->length + bufflen - string->capacity) + 1));

		if(new_buffer == NULL) {
			logger(ERROR, stdout, NULL, "%s: Не удалось выделить память",__func__);
			return -1;
		}

		string->buffer = new_buffer;
		string->capacity = string->capacity + (string->length + bufflen - string->capacity) + 1;
	}

	// посимвольно переносим строку в буфер
	size_t i, j;
	for(i = string->length, j = 0; buff[j] != '\0' && i < string->capacity; i++, j++) string->buffer[i] = buff[j];

	string->buffer[i] = '\0';

	string->length += bufflen;

	return bufflen;
}


ssize_t dstr_multi_append(dstr_t *string, size_t n, ...)
{
	// определяем список остаточных аргументов
	va_list strings;
	va_start(strings, n);

	ssize_t total = 0; // кол-во записанных символов

	// проходимся по списку аргументов
	for(size_t i = 0; i < n; i++) {
		ssize_t n = dstr_append(string, va_arg(strings, char *));

		if(n == -1) {
			logger(ERROR, stdout, NULL, "%s: Произошла ошибка при множественной вставке",__func__);
			return -1;
		}

		total += n;
	}

	va_end(strings);

	return total;
}


void dstr_log(dstr_t *string)
{
	puts(string->buffer);
}