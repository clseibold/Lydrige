#pragma once

#include <stdlib.h>
#define _CRT_SECURE_NO_WARNINGS

typedef size_t hash_size;

struct hashnode_s {
	char* key;
	void *data;
	struct hashnode_s* next;
};

typedef struct hashtbl {
	hash_size size;
	struct hashnode_s** nodes;
	hash_size (*hashfunc)(const char*);
} HASHTBL;

HASHTBL* hashtbl_create(hash_size size, hash_size(*hashfunc)(const char*));
void hashtbl_destroy(HASHTBL* hashtbl);
int hashtbl_insert(HASHTBL* hashtbl, const char* key, void *data);
int hashtbl_remove(HASHTBL* hashtbl, const char* key);
void* hashtbl_get(HASHTBL* hashtbl, const char* key);
int hashtbl_resize(HASHTBL* hashtbl, hash_size size);
HASHTBL* hashtbl_copy(HASHTBL* hashtbl);
