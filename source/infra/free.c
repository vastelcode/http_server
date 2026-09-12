// Модуль для освобождения памяти

#include <stdlib.h>

#include "../shared/types.h"
#include "../shared/prototypes.h"
#include "../shared/constants.h"

void free_to_count(void **array, size_t count)
{
	if(array) {
		for(size_t i = 0; i < count; i++) {
			if(array[i]) {
				free(array[i]);
				array[i] = NULL;
			}
		}
		free(array);
		array = NULL;
	}
}

void free_to_null(void **array)
{
	if(array) {
		for(size_t i = 0; array[i] != NULL; i++) {
			free(array[i]);
			array[i] = NULL;
		}
		free(array);
		array = NULL;
	}
}

void free_http(http_request *req)
{
	if(req) {
		if(req->method) free(req->method);
		if(req->url) free(req->url);
		if(req->version) free(req->version);
		if(req->body) free(req->body);
		if(req->headers) free_hashmap(req->headers);
		
		free(req);
	}
}