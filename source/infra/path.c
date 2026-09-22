#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../shared/logger.h"
#include "../shared/types.h"
#include "../shared/hashmap.h"

/** Таблица MIME-типов */
static const char *mime_types[][2] = {
    {"aac",    "audio/aac"},
    {"avi",    "video/x-msvideo"},
    {"bin",    "application/octet-stream"},
    {"bmp",    "image/bmp"},
    {"css",    "text/css"},
    {"csv",    "text/csv"},
    {"doc",    "application/msword"},
    {"docx",   "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {"eot",    "application/vnd.ms-fontobject"},
    {"epub",   "application/epub+zip"},
    {"gif",    "image/gif"},
    {"gz",     "application/gzip"},
    {"htm",    "text/html"},
    {"html",   "text/html"},
    {"ico",    "image/x-icon"},
    {"jpeg",   "image/jpeg"},
    {"jpg",    "image/jpeg"},
    {"js",     "application/javascript"},
    {"json",   "application/json"},
    {"jsonld", "application/ld+json"},
    {"mjs",    "application/javascript"},
    {"mp3",    "audio/mpeg"},
    {"mp4",    "video/mp4"},
    {"mpeg",   "video/mpeg"},
    {"ogg",    "audio/ogg"},
    {"ogv",    "video/ogg"},
    {"otf",    "font/otf"},
    {"pdf",    "application/pdf"},
    {"png",    "image/png"},
    {"ppt",    "application/vnd.ms-powerpoint"},
    {"pptx",   "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {"ps",     "application/postscript"},
    {"rar",    "application/vnd.rar"},
    {"rtf",    "application/rtf"},
    {"svg",    "image/svg+xml"},
    {"tar",    "application/x-tar"},
    {"tif",    "image/tiff"},
    {"tiff",   "image/tiff"},
    {"ttf",    "font/ttf"},
    {"txt",    "text/plain"},
    {"wav",    "audio/wav"},
    {"weba",   "audio/webm"},
    {"webm",   "video/webm"},
    {"webp",   "image/webp"},
    {"woff",   "font/woff"},
    {"woff2",  "font/woff2"},
    {"xls",    "application/vnd.ms-excel"},
    {"xlsx",   "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {"xml",    "application/xml"},
    {"zip",    "application/zip"},
    {NULL, NULL}
};


int path_is_file(const char *path)
{
	// 1. Инициализируем структуру статической информации
	struct stat fs = {0};

	// 2. Получаем статическую информацию
	if(stat(path, &fs) != 0) return 0;

	// 3. Определение
	if(S_ISREG(fs.st_mode) == 0) return 0;

	return 1;
}

int path_is_dir(const char *path)
{
	// 1. Инициализируем структуру статической информации
	struct stat fs = {0};

	// 2. Получаем статическую информацию
	if(stat(path, &fs) != 0) return 0;

	// 3. Определение
	if(S_ISDIR(fs.st_mode) == 0) return 0;

	return 1;
}

/* --- Внутренние хелперы --------------------------------------------- */

static int hex_val(char c);

/* --- Публичный API -------------------------------------------------- */

status_exec path_url_decode(const char *input, char *out, size_t out_size)
{
    if (!input || !out || out_size == 0) {
        logger(ERROR, stdout, NULL, "%s: Некорректные аргументы", __func__);
        return fail;
    }

    size_t i = 0;   /* индекс чтения во input  */
    size_t o = 0;   /* индекс записи в out     */

    while (input[i] != '\0') {

        if (o + 1 >= out_size) {
            logger(ERROR, stdout, NULL, "%s: Буфер переполнен", __func__);
            return fail;
        }

        if (input[i] != '%') {
            out[o++] = input[i++];
            continue;
        }

        /* '%' должен сопровождаться ровно двумя hex-цифрами */
        if (input[i + 1] == '\0' || input[i + 2] == '\0') {
            logger(ERROR, stdout, NULL,
                   "%s: Обрезанная escape-последовательность", __func__);
            return fail;
        }

        int hi = hex_val(input[i + 1]);
        int lo = hex_val(input[i + 2]);

        if (hi < 0 || lo < 0) {
            logger(ERROR, stdout, NULL,
                   "%s: Некорректная escape-последовательность", __func__);
            return fail;
        }

        unsigned char decoded = (unsigned char)((hi << 4) | lo);

        if (decoded == '\0') {
            logger(ERROR, stdout, NULL,
                   "%s: Escape-последовательность %%00 запрещена", __func__);
            return fail;
        }

        out[o++] = (char)decoded;
        i += 3;
    }

    out[o] = '\0';
    return success;
}

/* --- Внутренние функции -------------------------------------------- */

/**
 * Возвращает числовое значение hex-цифры или -1, если символ ею не является.
 */
static int hex_val(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

char *path_get_extension(const char *path)
{
	// 1. Ищем индекс последней точки в пути
	ssize_t pos_last_dot = -1;

	for(size_t i = 0; path[i] != '\0'; i++) if(path[i] == '.') pos_last_dot = i;

	if(pos_last_dot == -1) {
		logger(ERROR, stdout, NULL, "%s: Не обнаружено точки в пути до файла",__func__);
		return NULL;
	}

	if(path[pos_last_dot + 1] == '\0') {
		logger(ERROR, stdout, NULL, "%s: Название файла не содержит расширения",__func__);
		return NULL;
	}

	// 2. Определяем длину расширения
	size_t extlen = 0;
	for(size_t i = pos_last_dot; path[i] != '\0'; i++) extlen++;

	// 3. Определяем и заполняем посимвольно строку, содержащую расширение файла
	char *ext = malloc(sizeof(char) * extlen);

	if(ext == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память",__func__);
		return NULL;
	}

	size_t j = 0;
	for(size_t i = pos_last_dot + 1; path[i] != '\0'; i++) ext[j++] = path[i];

	ext[j] = '\0';

	return ext;
}

HashMap *path_init_mime(void)
{
	// 1. Инициализируем хэш-таблицу
	HashMap *table = hashmap_init(DEFAULT_HASHMAP_CAPACITY);

	if(table == NULL) {
		logger(ERROR, stdout, NULL, "%s: Не удалось выделить память",__func__);
		return NULL;
	}

	// 2. Попарно заносим статические значения
	for(size_t i = 0; mime_types[i][0] != NULL; i++) {
		if(hashmap_add(&table, mime_types[i][0], mime_types[i][1]) == fail) {
			logger(WARN, stdout, NULL, "%s: Не удалось добавить MIME-тип в таблицу",__func__);
			continue;
		}
	}

	return table;
}

char *path_get_mime(HashMap *table, const char *path)
{
	if(table == NULL || path == NULL) return NULL;

	// 1. Получаем расширение файла
	char *ext = path_get_extension(path);
	if(ext == NULL) return NULL;

	// 2. Получаем MIME-тип из таблицы
	node_t *type = hashmap_get_node(table, ext);

	if(type == NULL) {
		logger(ERROR, stdout, NULL, "%s: Данный MIME-тип не поддерживается - \"%s\"",__func__, ext);
		free(ext);
		return NULL;
	}

	free(ext);
	
	return type->value;
}