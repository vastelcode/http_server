// Public API - infra/path.c

#ifndef PATH

#include "types.h"
#include "hashmap.h"

#define PATH

/**
 * @brief Проверка на то является ли ресурс по данному пути файлом
 * 
 * @param[in] path Путь до ресурса
 * 
 * @return 0 | 1
 */
int path_is_file(const char *path);
/**
 * @brief Проверка на то является ли ресурс по данному пути директорией
 * 
 * @param[in] path Путь до ресурса
 * 
 * @return 0 | 1
 */
int path_is_dir(const char *path);

/**
 * @brief Percent-декодирование строки URL.
 *
 * Заменяет каждую последовательность "%XX" (XX — две hex-цифры) на
 * соответствующий байт. Остальные символы копируются без изменений,
 * включая '+' (в URL-пути '+' — литерал, а не пробел).
 *
 * Последовательность "%00" отвергается: NUL внутри строки — почти
 * всегда попытка обхода проверок и ломает работу C-строк.
 * Некорректные или обрезанные escape-последовательности — тоже ошибка.
 *
 * @param[in]  input     Исходная строка (NUL-terminated, не NULL)
 * @param[out] out       Буфер для результата (не NULL)
 * @param[in]  out_size  Размер буфера в байтах (минимум 1)
 *
 * @return
 * - success при успешном выполнении
 * @return
 * - fail при некорректных аргументах, битой последовательности
 *        или переполнении буфера
 */
status_exec path_url_decode(const char *input, char *out, size_t out_size);

/**
 * @brief Канонизирует HTTP-путь в виртуальный корень.
 *
 * Убирает повторные '/', сегменты '.' и '..', схлопывает пустые сегменты.
 * Результат всегда абсолютный (начинается с '/') и НИКОГДА не выходит
 * выше корня: последовательность '/../' на верхнем уровне — no-op.
 *
 * @param[in]  input     Исходный путь (произвольная строка, не NULL)
 * @param[out] out       Буфер для результата (не NULL)
 * @param[in]  out_size  Размер буфера; минимум 2 байта
 *
 * @return
 * - success при успешном выполнении
 * @return
 * - fail при некорректных аргументах или переполнении буфера
 */
status_exec path_realpath(const char *input, char *out, size_t out_size);

/**
 * @brief Получает расширение файла
 * 
 * @param[in] path Путь до файла
 * 
 * @return NULL | "ext"
 * 
 * @note Присутствует динамическое выделение памяти для расширения
 */
char *path_get_extension(const char *path);

/**
 * @brief Инициализация хэш-таблицы MIME-типов
 * 
 * @return NULL | Указатель на структуру HashMap
 * 
 * @note Присутствует динамическое выделение памяти
 */
HashMap *path_init_mime(void);

/**
 * @brief Получает MIME-тип файла
 * 
 * @param[in] path Путь до файла
 * @param[in] table Указатель на таблицу MIME-типов
 * 
 * @return NULL | MIME-тип
 */
char *path_get_mime(HashMap *table, const char *path);

#endif