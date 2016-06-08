#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "include/json.h"
#include "include/ac_json.h"

#if 0
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

static void *tree_create_structure(int nesting, int is_object)
{
	json_val_t *v = (json_val_t *)malloc(sizeof(json_val_t));
	if (v) {
		/* instead of defining a new enum type, we abuse the
		 * meaning of the json enum type for array and object */
		if (is_object) {
			v->type = JSON_OBJECT_BEGIN;
			v->u.object = NULL;
		} else {
			v->type = JSON_ARRAY_BEGIN;
			v->u.array = NULL;
		}
		v->length = 0;
	}
	return v;
}

static char *memalloc_copy_length(const char *src, uint32_t n)
{
	char *dest;

	dest = (char *)calloc(n + 1, sizeof(char));
	if (dest)
		memcpy(dest, src, n);
	return dest;
}

static void *tree_create_data(int type, const char *data, uint32_t length)
{
	json_val_t *v;

	v = (json_val_t*)malloc(sizeof(json_val_t));
	if (v) {
		v->type = type;
		v->length = length;
		v->u.data = memalloc_copy_length(data, length);
		if (!v->u.data) {
			free(v);
			return NULL;
		}
	}
	return v;
}

static int tree_append(void *structure, char *key, uint32_t key_length, void *obj)
{
	json_val_t *parent = (json_val_t *)structure;
	if (key) {
		struct json_val_elem *objelem;

		if (parent->length == 0) {
			parent->u.object = (struct json_val_elem**)calloc(1 + 1, sizeof(json_val_t *)); /* +1 for null */
			if (!parent->u.object)
				return 1;
		} else {
			uint32_t newsize = parent->length + 1 + 1; /* +1 for null */
			void *newptr;

			newptr = realloc(parent->u.object, newsize * sizeof(json_val_t *));
			if (!newptr)
				return -1;
			parent->u.object = (struct json_val_elem**)newptr;
		}

		objelem = (struct json_val_elem*)malloc(sizeof(struct json_val_elem));
		if (!objelem)
			return -1;

		objelem->key = memalloc_copy_length(key, key_length);
		objelem->key_length = key_length;
		objelem->val = (json_val_t*)obj;
		parent->u.object[parent->length++] = objelem;
		parent->u.object[parent->length] = NULL;
	} else {
		if (parent->length == 0) {
			parent->u.array = (struct json_val **)calloc(1 + 1, sizeof(json_val_t *)); /* +1 for null */
			if (!parent->u.array)
				return 1;
		} else {
			uint32_t newsize = parent->length + 1 + 1; /* +1 for null */
			void *newptr;

			newptr = realloc(parent->u.object, newsize * sizeof(json_val_t *));
			if (!newptr)
				return -1;
			parent->u.array = (struct json_val **)newptr;
		}
		parent->u.array[parent->length++] = (struct json_val *)obj;
		parent->u.array[parent->length] = NULL;
	}
	return 0;
}

static int process_data(json_parser *parser, char *data, int32_t data_len)
{
    uint32_t    processed;

	return json_parser_string(parser, data, data_len, &processed);
}

int do_tree(json_config *config, char *data, int32_t data_len, json_val_t **root_structure)
{
	json_parser parser;
	json_parser_dom dom;
	int ret;

	ret = json_parser_dom_init(&dom, tree_create_structure, tree_create_data, tree_append);
	if (ret) {
		DEBUGP("%s: error: initializing helper failed: errno: %d\n", __FUNCTION__, ret);
		return ret;
	}

	ret = json_parser_init(&parser, config, json_parser_dom_callback, &dom);
	if (ret) {
		DEBUGP("%s: error: initializing parser failed: errno: %d\n", __FUNCTION__, ret);
		return ret;
	}

	ret = process_data(&parser, data, data_len);
	if (ret) {
		DEBUGP("%s: error: can not process data\n", __FUNCTION__);
		return 1;
	}

	ret = json_parser_is_done(&parser);
	if (!ret) {
		DEBUGP("%s: error: syntax error\n", __FUNCTION__);
		return 1;
	}

	if (root_structure)
		*root_structure = (json_val_t *)dom.root_structure;

	json_parser_free(&parser);
    json_parser_dom_free(&dom);
	return 0;
}

