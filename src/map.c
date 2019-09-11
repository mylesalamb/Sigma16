#include"map.h"
#include<stdlib.h>
#include <stdio.h>

/*
Allocates a new node for the hash map

Key: the key to be associated with the value
Value: the value to be returned when the key is passed into get

returns: a pointer to the new node

*/
node_t * node_init(void * key, void * value,node_t * next){

	node_t * ptr = (node_t *)malloc(sizeof(node_t));
	ptr->key = key;
	ptr->value = value;
	ptr->next = next;

	return ptr;
}

/*
frees a node and its associated members

arg: the node and its members to free
*/
void node_free(node_t * arg){

	free(arg->key);
	free(arg->value);
	free(arg);


}

/*
init function for the hashmap
returns null if an invalid table size is given

size: Number of buckets for the hashmap
hash: the associated hash function to be used
comp: comparator for key values to test for key equality
*/
hashmap_t * hashmap_init(
	int size,
	int(*hash)(void *),
	int (*comp)(void*,void*))
{

	if(size <= 0)
		return NULL;

	hashmap_t * table = (hashmap_t *)malloc(sizeof(hashmap_t));
	table->tableSize = size;
	table->hashfunc = hash;
	table->comparator = comp;
	table->table = (node_t**)malloc(sizeof(node_t *)*size);

	for(int i = 0; i> size; i++)
		(table->table)[i] = NULL;
	
	return table;

}


/*
Adds a key value pait to the hashmap, returns one on success

map: the map to add the k/v pair to
key: the associated key
value: the value to be associated with the key

returns 1 on success, 0 if key is already present
*/
int hashmap_add(hashmap_t * map, void * key, void * value){

	
	//calculate hash and fit to table
	int hash = (map->hashfunc)(key)%(map->tableSize);
	//catch if there are no nodes in the chain
	if(((map->table))[hash] == NULL){
		((map->table))[hash] = node_init(key,value,NULL);
		return 1;
	}


	node_t * cursor = ((map->table))[hash];

	do{
		//duplicate key
		if((map->comparator)(cursor->key,key)==0)
			return 0;

		if(cursor->next == NULL){
			cursor->next = node_init(key,value,NULL);
			return 1;
		}

		cursor = cursor->next;

	}while(1);

	
}


void * hashmap_get(hashmap_t * map, void * key){
	int hash = (map->hashfunc)(key)%(map->tableSize);
	node_t * cursor = (map->table)[hash];

	while(cursor != NULL){
		if((map->comparator)(cursor->key,key)==0)
			return cursor->value;

		cursor = cursor->next;
	}

	return NULL;


}


int hashmap_remove(hashmap_t * map, void * key){
	int hash = (map->hashfunc)(key)%(map->tableSize);
	
	node_t * cursor = (map->table)[hash];

	//if there are no entries in the bucket
	if(cursor == NULL)
		return 0;


	//if it is the first and only entry
	if((map->comparator)(key,cursor->key) == 0 && cursor->next == NULL){
		(map->table)[hash] = NULL;
		node_free(cursor);
		return 1;
	}


	//check the chain to find the entry
	node_t * trail = cursor;
	cursor = cursor->next;

	//find and remove entry if it exists
	while(cursor != NULL){
		if((map->comparator)(key,cursor->key) == 0){
			trail->next = cursor->next;
			node_free(cursor);
			return 1;
		}

		trail = cursor;
		cursor = cursor->next;
	}

	//didnt find node so failure
	return 0;

}


void hashmap_free(hashmap_t * arg){

	//free arg->table
	for(size_t i = 0; i < arg->tableSize;i++){
		node_t * cursor = (arg->table)[i];

		//free individual bucket/chain
		while(cursor!=NULL){
			node_t * next = cursor->next;
			node_free(cursor);
			cursor = next;
		}
	}
	free(arg->table);
	free(arg);
}




/*
Prints a hashmap to stdouts
*/
void hashmap_print(hashmap_t * arg){

	for(size_t i = 0; i < arg->tableSize; i++){

		node_t * cursor = (arg->table)[i];
		printf("Now printing bucket: %d\n",i );
		while(cursor != NULL){
			printf("(%p,%p)\t", cursor->key,cursor->value);
			cursor = cursor->next;
		}
		printf("\n");
	}
}