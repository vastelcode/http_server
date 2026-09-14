// Public API - infra/hashmap.c
#ifndef HASHMAP

#include "types.h"
#include "constants.h"

#define HASHMAP

/**
 * @brief Освобождение узла связного списка
 *
 * @param[in, out]  node  Указатель на структуру, подлежащую освобождению
 */
void free_node(node_t *node);

/**
 * @brief Освобождение хэш-таблицы
 * 
 * @param[in,out] hashmap  Указатель на структуру, подлежащую освобождению
 * @note Полное освобождение структуры: каждый узел hashmap->data, указатель hashmap->data, указатель hashmap
 */
void free_hashmap(HashMap *hashmap);

/**
 * @brief Инициализация хэш-таблицы
 *
 * @param[in]  capacity Начальное количество ячеек в массиве
 *
 * @return
 * - Указатель на структуру HashMap в случае успеха
 * @return
 * - NULL в случае ошибки
 */
HashMap *hashmap_init(size_t capacity);

/**
 * @brief Добавление или обновление пары ключ-значение в хэш-таблице
 *
 * @param[in,out]  hashmap  Указатель на указатель структуры хэш-таблицы
 * @param[in]      key      Указатель на строку с ключом
 * @param[in]      value    Указатель на строку со значением
 *
 * @return
 * - success при успешном выполнении
 * @return
 * - fail в случае ошибки
 *
 * @note Если ключ уже существует, значение обновляется. Если новое значение
 *       совпадает со старым, функция завершается успешно без перезаписи.
 *       При заполнении таблицы автоматически выполняется рехеширование.
 * @see hashmap_rehash
 * @see hashmap_get_node
 * @see hashmap_create_node
 */
status_exec hashmap_add(HashMap **hashmap, char *key,  char *value);

/**
 * @brief Удаление узла по ключу
 *
 * @param[in,out]  hashmap  Указатель на структуру хэш-таблицы
 * @param[in]      key      Указатель на строку с ключом
 *
 * @return
 * - success при успешном выполнении
 * @return
 * - fail в случае ошибки
 *
 * @note При успешном удалении освобождается память, занятая узлом, через
 *       free_node, а связи в цепочке коллизий восстанавливаются.
 * @see hashmap_get_node
 * @see free_node
 */
status_exec hashmap_delete(HashMap *hashmap, char *key);

/**
 * @brief Получение узла по ключу
 *
 * @param[in]  hashmap  Указатель на структуру хэш-таблицы
 * @param[in]  key      Указатель на строку с ключом
 *
 * @return
 * - Указатель на найденный узел node_t в случае успеха
 * @return
 * - NULL в случае ошибки или отсутствия узла
 *
 * @note Поиск выполняется по цепочке коллизий в бакете, индекс которого
 *       вычисляется через hashmap_get_index.
 * @see hashmap_get_index
 * @see hashmap_get_hash
 */
node_t *hashmap_get_node(HashMap *hashmap, char *key);

/**
 * @brief Вывод содержимого хэш-таблицы в стандартный поток вывода
 *
 * @param[in]  hashmap  Указатель на структуру хэш-таблицы
 *
 * @note Функция предназначена для отладочного вывода. Для каждого непустого
 *       бакета выводится его индекс и все узлы связного списка в формате:
 *       "(%ld) Node: %s %s\n".
 * @see hashmap_add
 * @see hashmap_get_node
 */
void hashmap_log(HashMap *hashmap);

#endif