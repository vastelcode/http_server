// Модуль вывода сообщений
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <stdarg.h>

#include "../shared/logger.h"
#include "../shared/constants.h"


void logger(status_log status ,FILE *logfile ,pthread_mutex_t *mutex ,char *format , ...)
{
	if(format == NULL || logfile == NULL) return;

	// 1. Определяем текущее время
	time_t now = time(NULL);
	struct tm *ltm = localtime(&now);

	// 2. Определяем префиксное сообщение
	char *pref = (status == DEBUG) ? "DEBUG" : ((status == WARN) ? "WARNING" : ((status == INFO) ? "INFO" : "ERROR"));

	// 3. Формируем превью вывода
	char preview[64];
	sprintf(preview, "[%d::%d::%d] [%s]", ltm->tm_hour, ltm->tm_min, ltm->tm_sec, pref);

	// 4. Выводим сообщение

	// инициализируем список остаточных параметров
	va_list args;
	va_start(args, format);
	
	// формируем сообщение
	char message[1024];
	vsnprintf(message, sizeof(message), format, args);

	if(mutex) pthread_mutex_lock(mutex); // блокируем мьютекс

	fprintf(logfile, "%s %s\n", preview, message);

	if(mutex) pthread_mutex_unlock(mutex); // разблокировываем мьютекс

	va_end(args);
}