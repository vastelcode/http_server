// Public API - utils.c
#ifndef UTILS

#include <stdio.h>

#define UTILS

char **split_by_subs(char *buffer, const char *delim, size_t max_amount);
long extract_long(char *string);
char *to_lower(const char *s);
size_t count_to_null(void **array);

#endif