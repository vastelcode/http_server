// Public API - core

#ifndef CORE

#include "constants.h"
#include "types.h"

#define CORE

/**
 * @brief Запуск сервера на основе конфигурации
 * 
 * Создание пула потоков, инициализация очереди, запуск приёма соединений, добавление обработчика события SIGINT
 * 
 * @param[in,out] config  Указатель на хэш-таблицу, содержающую параметры конфигурации сервера
 * 
 * @return success | fail
 * @note После окончания работы вся используемая память, в том числе и config, корректно освобождается.
 */
status_exec server_run(HashMap *config);

/**
 * @brief Директорирование задачи
 * 
 * Определяет нужный обработчик задачи (static/cgi)
 *
 * @param[in]  req Указатель на структуру HTTP-запроса
 * @param[in]  config Указатель на хэш-таблицу, содержающую параметры конфигурации сервера
 * @param[in]  client_fd Дескриптор для общения с клиентом
 * 
 * @return Код ответа
 *
 * @warning В случае успеха, функция должна вернуть OK
 */
http_code_t dispatch_request(HttpRequest *req, HashMap *config, int client_fd);


#endif