/*
Author: Myles Lamb

Parser related utilities for the sigma16 emulator
translates asm into opcodes for easier manipulation by the emulator
so we can retain the 'code as data' behaviour see (strange.asm.txt) 
*/

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include"map.h"
#include"parser.h"

#define BUFFSIZE 200
#define MEMSIZE 65536


/*
initialises the op code mapping for the parser
ie) sub -> 0x1000

*/
static hashmap_t * opmapinit(){

	char * opchar;
	uint16_t * opcode;

	hashmap_t * mapping = hashmap_init(23,&strhash,&strcomp);

	//creating the mapping

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "add";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x0000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "sub";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x1000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "sub";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x2000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "div";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x3000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	opchar = "cmplt";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x4000;
	hashmap_add(mapping,opchar,opcode);

	return mapping;
}


/*
parses a sigma16 asm file returning an array of
16bit integers resolving references to labels


*/
uint16_t * getobjcode(FILE * fp){

char * buffer = (char*)malloc(sizeof(char)*BUFFSIZE);
//maybe use a struct and keep line numbering with the instructions
//for use within the debugger at a later stage?
uint16_t * mem = (uint16_t*)malloc(sizeof(uint16_t)*MEMSIZE);



}

/*
simple string hashing function
*/
int strhash(void * arg){
	char * c = (char *)arg;
	int sum = 0;
	while(*c != '\0')
		sum += *c;

	return sum;

}
/*
string comparator using void pointers
for use in the hash map
*/
int strcomp(void * arga, void * argb ){
	char * stra = (char *)arga;
	char * strb = (char *)argb;

	while(*stra != '\0' && *strb != '\0' && *stra == *strb){
		stra++;
		strb++;
	}

	return *stra - *strb;

}