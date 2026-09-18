#include <unistd.h>
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