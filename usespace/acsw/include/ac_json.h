#ifndef __AC_JSON_H__
#define __AC_JSON_H__

#include "json.h"

struct json_val_elem 
{
	char *key;
	uint32_t key_length;
	struct json_val *val;
};

typedef struct json_val 
{
	int type;
	int length;
	union 
    {
		char *data;
		struct json_val **array;
		struct json_val_elem **object;
	} u;
} json_val_t;


extern int do_tree(json_config *config, char *data, int32_t data_len, json_val_t **root_structure);


#endif

