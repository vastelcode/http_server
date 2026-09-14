// Public API - infra/dstr.c
#ifndef DSTR

#include <stdio.h>
#include "types.h"

#define DSTR

void free_dstr(dstr_t *dstr);

/**
 * @brief Инициализация динамической строки
 *
 * @param[out]  string  Указатель на изменяемую структуру динамической строки
 * @param[in]  capacity Начальная вместимость строки
 * 
 * @return 
 * - -1 в случае ошибки
 * @return 
 * - 0 в случае успеха
 *
 * @note Выделяется память из кучи для поля buffer
 * @warning Указатель string должен быть заранее определённым
 */
ssize_t dstr_init(dstr_t *string, size_t capacity);

/**
 * @brief Вставка строки в конец буфера динамической строки
 *
 * @param[out]  string  Указатель на изменяемую структуру динамической строки
 * @param[in]  buff Указатель на строку для вставки
 * 
 * @return 
 * - -1 в случае ошибки
 * @return 
 * - кол-во вставленных символов в случае успеха
 *
 * @note Поле capacity изменяется ровно на столько символов, сколько нехватает для вставки
 * @warning Указатель string должен быть заранее определённым
 */
ssize_t dstr_append(dstr_t *string, const char *buff);


/**
 * @brief Вставка нескольких строк в конец буфера динамической строки
 *
 * @param[out]  string  Указатель на изменяемую структуру динамической строки
 * @param[in]  n Количество строк для вставки
 * @param[in] buff Строки для вставки
 * 
 * @return 
 * - -1 в случае ошибки
 * @return 
 * - кол-во вставленных символов в случае успеха
 *
 * @note Поле capacity изменяется ровно на столько символов, сколько нехватает для вставки
 * @warning Указатель string должен быть заранее определённым
 */
ssize_t dstr_multi_append(dstr_t *string, size_t n, ...);
void dstr_log(dstr_t *string);

#endif