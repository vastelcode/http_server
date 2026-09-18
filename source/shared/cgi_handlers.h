#ifndef CGI_HANDLERS

#include "hashmap.h"
#include "http.h"

#define CGI_HANDLERS

/**
 * @brief Проверка на то, является ли файл на пути внешним скриптом
 * 
 * @param[in] req Указатель на структуру HTTP-запроса
 * @param[in] config Указатель на таблицу конфигурации
 * 
 * @return 0 - нет | 1 - да
 * 
 * @note Передаваемый путь должен быть нормализован и декодирован до вызова функции
 */
int cgi_check(HttpRequest *req, HashMap *config);

/**
 * @brief Обработчика запроса, запускающего CGI-скрипт
 * 
 * @param[in] req Указатель на структуру HTTP-запроса
 * @param[in] fullpath Полный путь к запрашиваему файлу
 */
status_exec cgi_handler(task_t *task, HashMap *config);

#endif