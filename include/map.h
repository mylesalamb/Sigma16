/*
Reference implementation for a generic HashMap in c
Author: Myles Lamb
*/

#ifndef MAP_GUARD
#define MAP_GUARD
#include<stdlib.h>

typedef struct Node{

	void * key;
	void * value;
	struct Node * next;

} node_t;

node_t * node_init(void * key, void * value,node_t *next);
void node_free(node_t * arg);




typedef struct HashMap
{

	int (*hashfunc)(void *);
	int (*comparator)(void *,void *);
	size_t tableSize;
	node_t ** table;
	
}hashmap_t;

hashmap_t * hashmap_init(int size,int(*hash)(void *),int (*comp)(void*,void*));
void * hashmap_get(hashmap_t * map, void * key);
int hashmap_add(hashmap_t * map, void * key, void * value);
int hashmap_remove(hashmap_t * map, void * key);
void hashmap_free(hashmap_t * map);
void hashmap_print(hashmap_t * map);


#endif