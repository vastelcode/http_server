#ifndef CONSTANTS

#include <limits.h>
#include <stdint.h>

#define CONSTANTS

typedef enum {
	DEFAULT_SIZE_BUFFER = 1024, // размер буфера для записи
	DEFAULT_HASHMAP_CAPACITY = 4, // базовая вместимость хэш-таблицы
	BASE_LENGTH_DSTR = 64, // базовая длина динамической строки
	BATCH_SIZE = 16 * 1024 // размер порции отправляемых данных
} sizes;

typedef enum {
	// кол-во потоков
	min_amount_threads = 1,
	max_amount_threads = 16,

	// кол-во одновременных подключений
	min_backlog = 1,
	max_backlog = 20,

	// номер порта
	min_port = 1024,
	max_port = UINT16_MAX,

	// размер тела запроса
	min_body_size = 0,
	max_body_size = UINT16_MAX,

	// размер пути запроса
	MAX_PATH_LEN = 2048,
	MAX_SIZE_HEADERS = 4096,
	MAX_BODY_SIZE = 64 * 1024,
	MAX_REQUEST_SIZE = MAX_PATH_LEN + MAX_SIZE_HEADERS + MAX_BODY_SIZE
} min_max;

typedef enum {
	default_amount_threads = 4, // кол-во потоков по умолчанию
	default_backlog = 5, // кол-во одновременных подключений по умолчанию
	default_port = 8080 // порт запуска сервера
} default_config;

typedef enum statuses {
	fail = -1,
	success = 0,
	warning = 1
} status_exec; // статусы выполнения

typedef enum {
	DEBUG,
	INFO,
	WARN,
	ERROR
} status_log; // статусы вывода

#endif