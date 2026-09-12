#ifndef PROTOTYPES

#include <sys/socket.h>
#include "constants.h"
#include "types.h"

#define PROTOTYPES

// infra/hashmap.c - модуль для работы с хэш-таблицей

void free_node(node_t *node);
void free_hashmap(HashMap *hashmap);
unsigned long hashmap_get_hash(char *string);
ssize_t hashmap_get_index(HashMap *hashmap, char *key);
HashMap *hashmap_init(size_t capacity);
status_exec hashmap_add(HashMap **hashmap, char *key,  char *value);
status_exec hashmap_delete(HashMap *hashmap, char *key);
node_t *hashmap_get_node(HashMap *hashmap, char *key);
status_exec hashmap_rehash(HashMap **hashmap, size_t new_size);
node_t *hashmap_create_node(char *key, char *value);
void hashmap_log(HashMap *hashmap);

// infa/dstr.c - модуль для работы с динамической строкой

void free_dstr(dstr_t *dstr);
ssize_t dstr_init(dstr_t *string, size_t capacity);
ssize_t dstr_append(dstr_t *string, const char *buff);
ssize_t dstr_multi_append(dstr_t *string, size_t n, ...);
void dstr_log(dstr_t *string);

// infra/queue.c - модуль для работы с очередью

void free_task(task_t *task);
void free_queue(Queue *queue);
Queue *queue_init(size_t capacity);
void queue_push(Queue *queue,task_t *task);
task_t *queue_shift(Queue *queue, int releace);

// infra/thread_pool.c - модуль для работы с пулом потоков

status_exec thread_pool_start(Context *context, int amount_threads);
status_exec thread_pool_preprocessing(task_t *task); // предобработка задачи (чтение запроса от клиента, формирование http, определение обработчика)
void thread_pool_submit(Context *context, task_t *task); // отправка задачи в очередь (обёртка над push_queue)
void *thread_pool_routine(void *arg); // функция потока (начало ожидания прихода задач)
status_exec thread_pool_stop(Context *context); // остановка пула потоков

// infra/config.c - модуль для работы с конфигурационным файлом

status_exec add_parameter(HashMap **hashmap,char *buffer); // добавление параметра в конфигурацию
int extract_param_long(HashMap *config, char *key, int default_val, int min_val, int max_val); // извлечение параметра конфигурации с числовым значением
HashMap *get_default_config(void); // возвращение хэш-таблицы с конфигурационными параметрами по умолчанию
HashMap *get_config(void); // чтение файла конфигурации и занесение значение в хэш-таблицу

// infra/http.c - модуль для работы с HTTP-запросами
http_request *parse_http(char *request); // парсинг HTTP-запроса: метод, путь, версия, заголовки и тело
char *translate_status_line(http_response *response); // запись статусной строки ответа в буфер
char *translate_headers(http_response *response); // запись всех заголовков ответа в буфер
char *translate_body(http_response *response, size_t size_body); // запись тела ответа в буфер
char *translate_http(http_response *response); // запись ответа клиента по структуре HTTP-ответа в буфер
char *fast_setting_response(http_response *res, unsigned short code, char *message); // быстрая настройка структуры HTTP-ответа и запись структуру в буфер
void log_http(http_request *req);

// infra/server.c - модуль для работы сервера

int create_server(const struct sockaddr *addr, int n); // создание сервера (socket, setsockopt, bind, listen)
ssize_t recv_all(void *buff,size_t buffsize ,int client_fd); // чтение всех данных из клиентского сокета в буфер
ssize_t send_all(void *buff,size_t buffsize ,int client_fd); // отправка всех данных из буфера клиенту


// infra/path.c - модуль для работы с путями

char *get_realpath(char *root, char *url); // получение канонизированного пути
int is_file(char *path); // проверка на существование файла
int is_dir(char *path); // проверка на существование директории
char *get_extension(char *path); // получение расширения файла

// infra/mime_type.c - модуль для определения MIME-типа
HashMap *init_mime_table(size_t capacity); // инициализация таблицы MIME-типов
char *get_mime_type(HashMap *mime_table, char *path); // получение MIME-типа

// infra/logger.c - модуль для вывода сообщений в терминал/файл

void logger(status_log status ,FILE *logfile ,pthread_mutex_t *mutex ,char *format , ...);

// handlers/get_handler.c - модуль для обработки GET-запросов

// handlers/post_handler.c - модуль для обработки POST-запросов

// utils.c - модуль утилитарных функций

char **split_by_subs(char *buffer, const char *delim, size_t max_amount); // разделение строки на токены по ПОДСТРОКЕ
long extract_long(char *string); // извлечение числа из строки
size_t count_to_null(void **array); // подсчёт элементов до NULL

// free.c - модуль освобождения памяти

void free_to_count(void **array, size_t count); // освобождение элементов массива по количеству
void free_to_null(void **array); // освобождение элементов массива до первого NULL
void free_http(http_request *req); // освобождение структуры http-запроса

#endif