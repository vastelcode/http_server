#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../shared/constants.h"
#include "../shared/http.h"
#include "../shared/hashmap.h"
#include "../shared/logger.h"

/* --- Внутренние прототипы ------------------------------------------ */

static status_exec parse_request_line(char *line, HttpRequest *req);
static status_exec parse_headers_block(char *block, HttpRequest *req);
static status_exec add_header(HttpRequest *req, const char *line, size_t len);
static void        strip_trailing_ws(const char *src, size_t len,
                                     const char **out, size_t *out_len);

/* --- Публичный API ------------------------------------------------- */

status_exec http_parse_request(const char *buf, size_t len, HttpRequest *req)
{
    if (!buf || !req || len == 0) {
        logger(ERROR, stdout, NULL, "%s: Некорректные аргументы", __func__);
        return fail;
    }

    memset(req, 0, sizeof(HttpRequest));
    req->method  = HTTP_METHOD_UNKNOWN;
    req->version = HTTP_VERSION_UNKNOWN;

    /* 1. Ищем границу заголовков "\r\n\r\n" */
    ssize_t header_end = http_find_header_end(buf, len);
    if (header_end < 0) {
        logger(ERROR, stdout, NULL, "%s: Не найден конец заголовков", __func__);
        return fail;
    }

    /* 2. Копируем секцию заголовков ВМЕСТЕ с завершающим \r\n,
     *    чтобы каждая строка была корректно терминирована. */
    size_t headers_len = (size_t)header_end + 2;
    char  *headers_buf = malloc(headers_len + 1);
    if (!headers_buf) {
        logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
        return fail;
    }
    memcpy(headers_buf, buf, headers_len);
    headers_buf[headers_len] = '\0';

    /* 3. Первая строка — request-line. Отрезаем её по первому CRLF. */
    char *crlf = strstr(headers_buf, "\r\n");
    if (!crlf) {
        free(headers_buf);
        logger(ERROR, stdout, NULL, "%s: Отсутствует CRLF", __func__);
        return fail;
    }
    *crlf = '\0';

    if (parse_request_line(headers_buf, req) == fail) {
        free(headers_buf);
        return fail;
    }

    /* 4. Инициализируем хэшмап заголовков */
    req->headers = hashmap_init(default_hashmap_capacity);
    if (!req->headers) {
        free(headers_buf);
        return fail;
    }

    /* 5. Разбираем оставшиеся строки заголовков */
    if (parse_headers_block(crlf + 2, req) == fail) {
        free(headers_buf);
        http_request_free(req);
        return fail;
    }
    free(headers_buf);

    /* 6. Тело запроса — всё, что после "\r\n\r\n" */
    size_t body_offset = (size_t)header_end + 4;
    if (body_offset < len) {
        size_t body_len = len - body_offset;

        req->body = malloc(body_len + 1);
        if (!req->body) {
            http_request_free(req);
            logger(ERROR, stdout, NULL, "%s: Не удалось выделить память", __func__);
            return fail;
        }
        memcpy(req->body, buf + body_offset, body_len);
        req->body[body_len] = '\0';
        req->body_len = body_len;
    }

    return success;
}

void http_request_free(HttpRequest *req)
{
    if (!req) return;

    if (req->headers) {
        free_hashmap(req->headers);
        req->headers = NULL;
    }
    if (req->body) {
        free(req->body);
        req->body = NULL;
    }
    req->body_len = 0;
}

/* --- Внутренние функции ------------------------------------------- */

/**
 * Разбирает строку вида: "GET /path?query HTTP/1.1".
 */
static status_exec parse_request_line(char *line, HttpRequest *req)
{
    char *method  = strtok(line, " ");
    char *target  = strtok(NULL, " ");
    char *version = strtok(NULL, " ");

    if (!method || !target || !version) {
        logger(ERROR, stdout, NULL, "%s: Некорректная строка запроса", __func__);
        return fail;
    }

    req->method = http_method_from_string(method);
    if (req->method == HTTP_METHOD_UNKNOWN) {
        logger(ERROR, stdout, NULL, "%s: Неизвестный метод", __func__);
        return fail;
    }

    req->version = http_version_from_string(version);
    if (req->version == HTTP_VERSION_UNKNOWN) {
        logger(ERROR, stdout, NULL, "%s: Неизвестная версия", __func__);
        return fail;
    }

    /* Разделяем path и query по '?' */
    char *qmark = strchr(target, '?');
    if (qmark) {
        size_t path_len = (size_t)(qmark - target);
        if (path_len >= max_path_len) return fail;

        memcpy(req->path, target, path_len);
        req->path[path_len] = '\0';

        const char *q = qmark + 1;
        size_t q_len = strlen(q);
        if (q_len >= max_path_len) return fail;

        memcpy(req->query, q, q_len + 1);
    } else {
        size_t path_len = strlen(target);
        if (path_len >= max_path_len) return fail;

        memcpy(req->path, target, path_len + 1);
        req->query[0] = '\0';
    }

    return success;
}

/**
 * Идёт по строкам заголовков, каждая из которых заканчивается "\r\n".
 * Пустая строка означает конец блока.
 */
static status_exec parse_headers_block(char *block, HttpRequest *req)
{
    char *cursor = block;

    while (*cursor != '\0') {
        char *line_end = strstr(cursor, "\r\n");
        if (!line_end) return fail;  // незавершённая строка

        size_t line_len = (size_t)(line_end - cursor);

        if (line_len == 0) break;    // пустая строка — конец заголовков

        if (add_header(req, cursor, line_len) == fail)
            return fail;

        cursor = line_end + 2;
    }
    return success;
}

/**
 * Парсит одну строку "Name: value" и добавляет её в хэшмап запроса.
 * Имя приводится к нижнему регистру (HTTP-заголовки регистронезависимы).
 */
static status_exec add_header(HttpRequest *req, const char *line, size_t len)
{
    const char *colon = memchr(line, ':', len);
    if (!colon || colon == line) return fail;

    size_t name_len = (size_t)(colon - line);

    const char *value;
    size_t      value_len;
    strip_trailing_ws(colon + 1, len - name_len - 1, &value, &value_len);

    char *name = malloc(name_len + 1);
    char *val  = malloc(value_len + 1);
    if (!name || !val) {
        free(name);
        free(val);
        return fail;
    }

    for (size_t i = 0; i < name_len; i++) {
        char c = line[i];
        name[i] = (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    }
    name[name_len] = '\0';

    memcpy(val, value, value_len);
    val[value_len] = '\0';

    /* hashmap_add копирует ключ и значение — свои буферы освобождаем */
    status_exec st = hashmap_add(&req->headers, name, val);

    free(name);
    free(val);
    return st;
}

/**
 * Обрезает ведущие и завершающие пробелы/табы вокруг значения.
 * Возвращает указатель внутрь исходного буфера и длину.
 */
static void strip_trailing_ws(const char *src, size_t len,
                              const char **out, size_t *out_len)
{
    while (len > 0 && (*src == ' ' || *src == '\t')) {
        src++;
        len--;
    }
    while (len > 0 && (src[len - 1] == ' ' || src[len - 1] == '\t')) {
        len--;
    }
    *out     = src;
    *out_len = len;
}