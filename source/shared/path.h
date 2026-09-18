// Public API - infra/path.c

#ifndef PATH

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

#endif