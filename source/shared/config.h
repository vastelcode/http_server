// Public API - infra/config.c
#ifndef CONFIG

#include "types.h"
#include "constants.h"

#define CONFIG

status_exec add_parameter(HashMap **hashmap,char *buffer);
int extract_param_long(HashMap *config, char *key, int default_val, int min_val, int max_val);
HashMap *get_default_config(void);
HashMap *get_config(void);

#endif