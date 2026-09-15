#include <stdlib.h>
#include <string.h>

#include "../shared/types.h"
#include "../shared/hashmap.h"

http_method_t http_method_from_string(const char *s)
{
    if (!s) return HTTP_METHOD_UNKNOWN;

    if (strcmp(s, "GET")     == 0) return HTTP_METHOD_GET;
    if (strcmp(s, "POST")    == 0) return HTTP_METHOD_POST;

    return HTTP_METHOD_UNKNOWN;
}

const char *http_method_to_string(http_method_t m)
{
    switch (m) {
        case HTTP_METHOD_GET:     return "GET";
        case HTTP_METHOD_POST:    return "POST";
        default:                  return "UNKNOWN";
    }
}

http_version_t http_version_from_string(const char *s)
{
    if (!s) return HTTP_VERSION_UNKNOWN;

    if (strcmp(s, "HTTP/1.0") == 0) return HTTP_VERSION_1_0;
    if (strcmp(s, "HTTP/1.1") == 0) return HTTP_VERSION_1_1;
    if (strcmp(s, "HTTP/2.0") == 0) return HTTP_VERSION_2_0;

    return HTTP_VERSION_UNKNOWN;
}

const char *http_version_to_string(http_version_t v)
{
    switch (v) {
        case HTTP_VERSION_1_0: return "HTTP/1.0";
        case HTTP_VERSION_1_1: return "HTTP/1.1";
        case HTTP_VERSION_2_0: return "HTTP/2.0";
        default:               return "HTTP/UNKNOWN";
    }
}

ssize_t http_find_header_end(const char *buf, size_t len)
{
    if (!buf || len < 4) return -1;

    for (size_t i = 0; i + 3 < len; i++) {
        if (buf[i]     == '\r' && buf[i + 1] == '\n' &&
            buf[i + 2] == '\r' && buf[i + 3] == '\n') {
            return (ssize_t)i;
        }
    }
    return -1;
}

/**
 * Приводит имя заголовка к нижнему регистру во временном буфере,
 * затем ищет узел в хэшмапе. Возвращает указатель на значение внутри
 * узла хэшмапа (не освобождать!) либо NULL.
 */
const char *http_get_header(const HttpRequest *req, const char *name)
{
    if (!req || !req->headers || !name) return NULL;

    size_t n = strlen(name);
    char  *lower = malloc(n + 1);
    if (!lower) return NULL;

    for (size_t i = 0; i < n; i++) {
        char c = name[i];
        lower[i] = (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    }
    lower[n] = '\0';

    node_t *node = hashmap_get_node(req->headers, lower);
    free(lower);

    return node ? node->value : NULL;
}