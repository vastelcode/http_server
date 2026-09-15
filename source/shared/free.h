// Public API - infra/free.c
#ifndef FREE

#include <stdio.h>
#include "types.h"

#define FREE

void free_to_count(void **array, size_t count); // освобождение элементов массива по количеству
void free_to_null(void **array); // освобождение элементов массива до первого NULL

#endif