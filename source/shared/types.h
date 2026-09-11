#ifndef TYPES

#include <stdio.h>
#include "constants.h"

#define TYPES


typedef struct node {
	char *key;
	char *value;
	struct node *next;
} node_t; // узел связного списка


typedef struct {
	size_t capacity;
	size_t amount;
	node_t **data;
} HashMap; // структура данных "Хэш-таблица"

typedef struct {
	char *version;
	char *method;
	char *url;
	HashMap *headers;
	char *body;
} http_request; // структура HTTP-запроса

typedef struct {
	char *version;
	char *status_message;
	unsigned short code;
	HashMap *headers;
	char *body;
} http_response; // структура HTTP-ответа

typedef struct task{
	int client_fd; // клиентский сокет
	http_request *request; // структура HTTP-запроса
	status_exec (* handler) (struct task *task, HashMap *config); // обработчик задачи
} task_t; // структура задачи

typedef struct {
	task_t **data;
	size_t capacity;
	size_t amount;
} Queue; // структура данных "Очередь"

typedef struct {
	Queue *tasks;
	HashMap *config;
	size_t created_threads;
	volatile int stop_requested;
} Context; // констекст для функционирования приложения


#endif