#ifndef CONSTANTS

#include <limits.h>
#include <stdint.h>

#define CONSTANTS

typedef enum {
	default_size_buffer_string = 1024, // размер буфера для записи
	default_length_token = 256, // размер подстроки
	default_hashmap_capacity = 4, // базовая вместимость хэш-таблицы
	default_queue_capaicty = 10, // базовая вместимость очереди
	default_body_size = 1024, // базовый размер тела запроса
	default_request_size = 2048, // базовая длина запроса
	base_length_dstr = 64 // базовая длина динамической строки
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
	max_body_size = UINT16_MAX
} min_max;

typedef enum {
	default_amount_threads = 4, // кол-во потоков по умолчанию
	default_backlog = 5, // кол-во одновременных подключений по умолчанию
	default_port = 8080 // порт запуска сервера
} default_config;

typedef enum {
	recv_val = 100,
	send_val = 200
} server_operation;

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