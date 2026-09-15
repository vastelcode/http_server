// Public API - http

#ifndef HTTP

#include <stddef.h>
#include <stdio.h>
#include "types.h"

#define HTTP

http_method_t   http_method_from_string (const char *s);
const char     *http_method_to_string   (http_method_t m);

http_version_t  http_version_from_string(const char *s);
const char     *http_version_to_string  (http_version_t v);

/** Ищет "\r\n\r\n". Возвращает индекс начала или -1. */
ssize_t http_find_header_end(const char *buf, size_t len);

/** Получить значение заголовка (регистронезависимо). NULL если нет. */
const char *http_get_header(const HttpRequest *req, const char *name);


/**
 * Разбирает сырой буфер с HTTP-запросом в структуру HttpRequest.
 *
 * При успехе req->headers будет инициализирован, а req->body (если есть)
 * выделен в куче. Освобождать через http_request_free.
 *
 * @return success | fail
 */
status_exec http_parse_request(const char *buf, size_t len, HttpRequest *req);

/** Освобождает все динамические ресурсы внутри запроса. */
void http_request_free(HttpRequest *req);

#endif