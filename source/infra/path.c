#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../shared/logger.h"

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

/* --- Внутренние хелперы --------------------------------------------- */

static status_exec append_segment(char *out, size_t out_size, size_t *len,
                                  const char *seg, size_t seg_len);

static status_exec pop_segment(char *out, size_t *len);

/* --- Публичный API -------------------------------------------------- */

status_exec path_realpath(const char *input, char *out, size_t out_size)
{
    if (!input || !out || out_size < 2) {
        logger(ERROR, stdout, NULL, "%s: Некорректные аргументы", __func__);
        return fail;
    }

    /* Всегда стартуем с корня. out_len — длина без учёта NUL. */
    out[0]     = '/';
    out[1]     = '\0';
    size_t out_len = 1;

    const char *p = input;

    while (*p) {
        while (*p == '/') p++;           /* глотаем разделители подряд */
        if (!*p) break;

        const char *start = p;
        while (*p && *p != '/') p++;
        size_t seg_len = (size_t)(p - start);

        if (seg_len == 1 && start[0] == '.') {
            continue;                    /* "." — ничего не делаем */
        }
        if (seg_len == 2 && start[0] == '.' && start[1] == '.') {
            if (pop_segment(out, &out_len) == fail) return fail;
            continue;                    /* ".." — на уровень вверх */
        }

        if (append_segment(out, out_size, &out_len, start, seg_len) == fail)
            return fail;
    }

    return success;
}

/* --- Внутренние функции -------------------------------------------- */

/**
 * Дописывает сегмент к результату, вставляя разделитель, если это
 * не первый сегмент после корня.
 */
static status_exec append_segment(char *out, size_t out_size, size_t *len,
                                  const char *seg, size_t seg_len)
{
    size_t sep  = (*len > 1) ? 1 : 0;   /* '/' перед сегментом, кроме корня */

    /* нужен слот под разделитель + сам сегмент + завершающий NUL */
    if (*len + sep + seg_len + 1 > out_size) {
        logger(ERROR, stdout, NULL, "%s: Буфер переполнен", __func__);
        return fail;
    }

    if (sep) out[(*len)++] = '/';

    memcpy(out + *len, seg, seg_len);
    *len += seg_len;
    out[*len] = '\0';

    return success;
}

/**
 * Срезает последний сегмент пути. На корне — no-op (нельзя выйти выше).
 */
static status_exec pop_segment(char *out, size_t *len)
{
    if (*len <= 1) return success;      /* уже в корне */

    while (*len > 1 && out[*len - 1] != '/')
        (*len)--;

    if (*len > 1) (*len)--;             /* снимаем сам '/' */
    out[*len] = '\0';

    return success;
}