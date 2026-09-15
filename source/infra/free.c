// Модуль для освобождения памяти

#include <stdlib.h>

#include "../shared/types.h"
#include "../shared/free.h"
#include "../shared/hashmap.h"
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