// Модуль, предоставляющий функции для работы с хэш-таблицей
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../shared/constants.h"
#include "../shared/prototypes.h"
#include "../shared/types.h"

void free_node(node_t *node)
{
	if(node) {
		if(node->key) free(node->key);
		if(node->value) free(node->value);
		free(node);

		node = NULL;
	}
}

void free_hashmap(HashMap *hashmap)
{
	for(size_t i = 0; i < hashmap->capacity; i++) {

		// получаем голову связного списка
		node_t *head = hashmap->data[i];

		if(!head) continue;

		node_t *next = NULL;

		// проходимся вниз по связному списку
		while(head) {
			next = head->next;
			free_node(head);
			head = next;
		}
	}

	free(hashmap->data);
	free(hashmap);
}

/**
 * @brief Высчитывает хэш для входной строки
 *
 * @param[in]  string  Ключ для хэш-таблицы
 *
 * @return
 * 	- 0 при ошибке
 * @return
 * 	- unsigned long, отличное от нуля, при успехе
 *
 * @see get_index
 */
unsigned long get_hash(char *string)
{
	if(string == NULL) {
		logger(ERROR ,stdout, NULL, "%s: Ключ имеет неопределённое значение",__func__);
		return 0;
	}

    unsigned long hash = 5381;
    for (size_t i = 0; string[i] != '\0'; i++)
        hash = ((hash << 5) + hash) + (unsigned)string[i];
    return hash;
}

/**
 * @brief Инициализация хэш-таблицы
 *
 * Подробное описание: что делает функция, зачем она нужна,
 * какие алгоритмы использует, какие особенности имеет.
 *
 * @param[in]  param1  Описание первого входного параметра.
 * @param[in]  param2  Описание второго входного параметра.
 * @param[out] result  Указатель, куда будет записан результат.
 * @param[in,out] buf  Буфер, который читается и модифицируется.
 *
 * @return Описание возвращаемого значения.
 *         - 0 при успехе
 *         - -1 при ошибке (errno устанавливается)
 *
 * @note Дополнительные замечания, предупреждения, ограничения.
 * @warning Важные предупреждения (например, не потокобезопасно).
 * @see related_function() Связанные функции.
 */
HashMap *init_hashmap(size_t capacity)
{
    HashMap *hashmap = calloc(1, sizeof(HashMap)); // выделяем память на структуру

    if (hashmap == NULL) {
        logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
        return NULL;
    }

    hashmap->data = calloc(capacity, sizeof(node_t *)); // выделяем память на массив данных

    if (hashmap->data == NULL) {
        logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
        free(hashmap);
        return NULL;
    }

    hashmap->amount = 0;
    hashmap->capacity = (capacity > 0) ? capacity : default_hashmap_capacity;
    return hashmap;
}

ssize_t get_index(HashMap *hashmap, char *key)
{
    if (hashmap == NULL || key == NULL) return -1;

    unsigned long hash = get_hash(key);
    return hash % hashmap->capacity;
}

node_t *create_node_hashmap(char *key, char *value)
{
    node_t *new_node = calloc(1, sizeof(node_t));

    if (new_node == NULL) {
        logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
        return NULL;
    }

    size_t keylen = strlen(key);
    size_t vallen = strlen(value);

    new_node->key = malloc(sizeof(char) * (keylen + 1));

    if (new_node->key == NULL) {
        logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
        free(new_node);
        return NULL;
    }

    new_node->value = malloc(sizeof(char) * (vallen + 1));

    if (new_node->value == NULL) {
        logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
        free(new_node->key);
        free(new_node);
        return NULL;
    }

    strncpy(new_node->key, key, keylen);
    new_node->key[keylen] = '\0';

    strncpy(new_node->value, value, vallen);
    new_node->value[vallen] = '\0';

    return new_node;
}

void log_hashmap(HashMap *hashmap)
{
    for (size_t i = 0; i < hashmap->capacity; i++) {
        node_t *node = hashmap->data[i];
        while (node) {
            printf("(%ld) Node: %s %s\n", i, node->key, node->value);
            node = node->next;
        }
    }
}

status_exec rehash_hashmap(HashMap **hashmap, size_t new_size)
{
    if (new_size <= 0) new_size = default_hashmap_capacity * 2;

    HashMap *new = init_hashmap(new_size); // выделяем память на новую таблицу

    if (new == NULL) return fail;

	// проходимся по каждому бакету старой таблицы и добавляем его в новую
    for (size_t i = 0; i < (*hashmap)->capacity; i++) {
        node_t *node = (*hashmap)->data[i];

        while (node) {
            
            if (add_hashmap(&new, node->key, node->value) == fail) {
                free_hashmap(new);
                return fail;
            }

            node = node->next;
        }
    }

    free_hashmap(*hashmap);
    *hashmap = new;   // обновляем внешний указатель

    return success;
}

node_t *get_node_hashmap(HashMap *hashmap, char *key)
{
    if (key == NULL) {
        logger(ERROR, stdout, NULL, "%s: Ключ имеет неопределённое значение", __func__);
        return NULL;
    }

    ssize_t index = get_index(hashmap, key); // получаем индекс

    if (index == -1)
        return NULL;

    node_t *node = hashmap->data[index]; // получаем конретный узел

	// спускаемся вниз по связному списку
    while (node) {
        if (strcmp(node->key, key) == 0)
            return node;
        node = node->next;
    }
    return NULL;
}


status_exec add_hashmap(HashMap **hashmap, char *key, char *value)
{
    if (key == NULL || value == NULL) {
        logger(ERROR, stdout, NULL, "%s: Ключ или значение неопределены", __func__);
        return fail;
    }

    // Если таблица заполнена – рехешируем, передавая адрес указателя
    if ((*hashmap)->capacity == (*hashmap)->amount) {
        if (rehash_hashmap(hashmap, (*hashmap)->capacity * 2) == fail)
            return fail;
    }

    ssize_t index = get_index(*hashmap, key);

    if (index == -1) {
        logger(ERROR, stdout, NULL, "%s: Не удалось получить индекс", __func__);
        return fail;
    }

    size_t vallen = strlen(value);

    // Проверяем, есть ли уже такой ключ
    node_t *founded = get_node_hashmap(*hashmap, key);
    if (founded) {

		if(strcmp(founded->value, value) == 0) return success; // если ключи совпадают

        // Обновляем значение
        free(founded->value);
        founded->value = malloc(sizeof(char) * (vallen + 1));

        if (!founded->value) {
            logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
            return fail;
        }
        strncpy(founded->value, value, vallen);
        founded->value[vallen] = '\0';
        return success;
    }

    // Вставка в конец списка
    node_t *current = (*hashmap)->data[index];
    node_t *prev = NULL;
    while (current) {
        prev = current;
        current = current->next;
    }

    node_t *new_node = create_node_hashmap(key, value);
    if (new_node == NULL)
        return fail;

    if (prev == NULL)
        (*hashmap)->data[index] = new_node;
    else
        prev->next = new_node;

    (*hashmap)->amount++;
    return success;
}

status_exec delete_hashmap(HashMap *hashmap, char *key)
{
    if (key == NULL) {
        logger(ERROR, stdout, NULL, "%s: Ключ имеет неопределённое значение", __func__);
        return fail;
    }

    node_t *target = get_node_hashmap(hashmap, key);

    if (!target) {
        logger(ERROR, stdout, NULL, "%s: Узел с таким ключом отсуствует", __func__);
        return fail;
    }

    ssize_t index = get_index(hashmap, key);

    if (index == -1) {
        logger(ERROR, stdout, NULL, "%s: Не удалось получить индекс", __func__);
        return fail;
    }

    node_t *current = hashmap->data[index];
    node_t *prev = NULL;

    while (current != target) {
        prev = current;
        current = current->next;
    }

    node_t *next = current->next;
    free_node(current);

    if (prev)
        prev->next = next;
    else
        hashmap->data[index] = next;

    hashmap->amount--;
    return success;
}
