#ifndef STATIC_HANDLERS

#include "types.h"

#define STATIC_HANDLERS

/**
 * @brief Обработчика запроса, запрашивающего содержимое файла
 * 
 * @param[in] req Указатель на структуру HTTP-запроса
 * @param[in] fullpath Полный путь к запрашиваему файлу
 */
status_exec static_handler(task_t *task, HashMap *config);

#endif