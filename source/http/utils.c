#include <stdlib.h>
#include <string.h>

#include "../shared/types.h"
#include "../shared/hashmap.h"
#include "../shared/utils.h"

http_method_t http_method_from_string(const char *s)
{
    if (!s) return HTTP_METHOD_UNKNOWN;

    if (strcmp(s, "GET")     == 0) return HTTP_METHOD_GET;
    if (strcmp(s, "POST")    == 0) return HTTP_METHOD_POST;
	if (strcmp(s, "PUT")    == 0) return HTTP_METHOD_PUT;
	if (strcmp(s, "DELETE")    == 0) return HTTP_METHOD_DELETE;

    return HTTP_METHOD_UNKNOWN;
}

const char *http_method_to_string(http_method_t m)
{
    switch (m) {
        case HTTP_METHOD_GET:     return "GET";
        case HTTP_METHOD_POST:    return "POST";
        case HTTP_METHOD_PUT:    return "PUT";
        case HTTP_METHOD_DELETE:    return "DELETE";
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

	char *lower_name = to_lower(name);
	if(lower_name == NULL) return NULL;

    node_t *node = hashmap_get_node(req->headers, lower_name);
    free(lower_name);

    return node ? node->value : NULL;
}

const char *http_code_to_string(http_code_t code)
{
    switch (code) {
        case OK:     return "OK";
        case NotFound:    return "Not Found";
        case NotAllowed:    return "Not Allowed";
		case ForBidden: return "For Bidden";
		case RequestEntityTooLarge: return "Request Entity Too Large";
		case BadRequest: return "Bad Request";
		case IntervalServerError: return "Interval Server Error";
        default:                  return "UNKNOWN";
    }
}

http_code_t http_code_from_string(const char *s) 
{
	if(!s) return HTTP_CODE_UNKWOWN;

	char *lower_s = to_lower(s);
	if(!lower_s) return HTTP_CODE_UNKWOWN;

	http_code_t target = HTTP_CODE_UNKWOWN;

	if(strcmp(lower_s, "ok") == 0) target = OK;
	else if(strcmp(lower_s, "not allowed") == 0) target = NotAllowed;
	else if(strcmp(lower_s, "not found") == 0) target = NotFound;
	else if(strcmp(lower_s, "bad request") == 0) target = BadRequest;
	else if(strcmp(lower_s, "for bidden") == 0) target = ForBidden;
	else if(strcmp(lower_s, "request entity too large") == 0) target = RequestEntityTooLarge;
	else if(strcmp(lower_s, "interval server error") == 0) target = IntervalServerError;

	free(lower_s);

	return target;
}