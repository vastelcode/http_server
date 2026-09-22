#ifndef TYPES

#include <stdio.h>
#include "constants.h"

#define TYPES

typedef struct {
	char *buffer; // указатель на буфер (NULL-terminated)
	size_t length; // текущая длина строки
	size_t capacity; // вместимость строки
} dstr_t; // динамическая строка

typedef struct node {
	char *key;
	char *value;
	struct node *next;
} node_t; // узел связного списка


typedef struct {
	size_t capacity; // кол-во ячеек в массиве
	size_t amount; // кол-во узлов в таблице
	node_t **data; // массив с данными
} HashMap; // структура данных "Хэш-таблица"

/** Методы HTTP-запроcа */
typedef enum {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_UNKNOWN
} http_method_t;

/** Версии HTTP-запроса */
typedef enum {
    HTTP_VERSION_1_0,
    HTTP_VERSION_1_1,
    HTTP_VERSION_2_0,
    HTTP_VERSION_UNKNOWN
} http_version_t;

/** HTTP-коды ответов */
typedef enum {
	OK = 200,
	NotFound = 404,
	NotAllowed = 405,
	BadRequest = 400,
	ForBidden = 403,
	RequestEntityTooLarge = 413,
	LengthRequired = 411, 
	IntervalServerError = 500,
	HTTP_CODE_UNKWOWN = -1
} http_code_t;

/**
 * Разобранный HTTP-запрос.
 *   method  — метод запроса
 *   path    — путь без query-строки
 *   query   — query-строка (без '?'), пустая если её нет
 *   version — версия протокола
 *   headers — HashMap: имя в нижнем регистре -> значение
 *   body    — тело запроса (NULL если отсутствует), всегда null-terminated
 */
typedef struct {
    http_method_t  method;
    char *path, *body;
	char query[MAX_PATH_LEN];
    http_version_t version;
    HashMap *headers;
    size_t body_len;
} HttpRequest;


/**
 * Структура HTTP-ответа.
 *   method  — метод ответа
 *	 status_code - код статуса
 *	 message -  краткое описание статуса
 *   version — версия протокола
 *   headers — HashMap: имя в нижнем регистре -> значение
 *   body    — тело ответа (NULL если отсутствует), всегда null-terminated
 */
typedef struct {
    http_code_t status_code;
    http_version_t version;
    HashMap *headers;
	size_t body_len;
    char *body, *message;
} HttpResponse;

typedef struct task {
	int client_fd; // клиентский сокет
	HttpRequest *request; // структура HTTP-запроса
	char *fullpath; // полный сформированный путь
	status_exec (* handler) (struct task *task, HashMap *mime_table); // обработчик задачи
} task_t; // структура задачи

typedef struct {
	task_t **data;
	size_t capacity;
	size_t amount;
} Queue; // структура данных "Очередь"

typedef struct {
	Queue *tasks; // очередь задач
	HashMap *config; // таблица конфигурации
	HashMap *mime_table; // таблица MIME-типов
	size_t created_threads; // кол-во созданных потоков
	volatile int stop_requested; // флаг остановки обработки запросов
} Context; // констекст для функционирования пула потоков


#endif