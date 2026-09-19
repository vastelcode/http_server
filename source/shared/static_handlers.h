#ifndef STATIC_HANDLERS

#include "types.h"

#define STATIC_HANDLERS

/**
 * @brief Обработчика запроса, запрашивающего содержимое файла
 * 
 * @param[in] task Указатель на структуру задачи
 * @param[in] mime_table Хэш-таблица MIME-типов
 * 
 * @return success | fail
 */
status_exec static_handler(task_t *task, HashMap *mime_table);

#endif