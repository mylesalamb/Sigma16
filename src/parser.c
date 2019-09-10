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
#define BUCKETS 10

/*
parses a sigma16 asm file returning an array of
16bit integers resolving references to labels


*/
uint16_t * getobjcode(FILE * fp){

hashmap_t * codebook = opmapinit();

char * buffer = (char*)malloc(sizeof(char)*BUFFSIZE);
//maybe use a struct and keep line numbering with the instructions
//for use within the debugger at a later stage?
uint16_t * mem = (uint16_t*)malloc(sizeof(uint16_t)*MEMSIZE);

return 0x0000;

}

/*
simple string hashing function
*/
int strhash(void * arg){
	printf("char val: %p",arg);
	char * c = (char *)arg;
	int sum = 0;
	while( (*c) != '\0'){
		
		sum += *c;
		c++;
	}

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

	return (*stra) - (*strb);

}


/*
initialises the op code mapping for the parser
ie) sub -> 0x1000

*/
static hashmap_t * opmapinit(){

	char * opchar;
	uint16_t * opcode;

	hashmap_t * mapping = hashmap_init(BUCKETS,&strhash,&strcomp);

	int status;

	//creating the mapping

	//RRR instructions

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
	opchar = "mul";
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

	opchar = (char *)malloc(sizeof(char)*6);
	opchar = "cmpeq";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x5000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	opchar = "cmpgt";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x6000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "cmp";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x7000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "inv";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x8000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "and";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x9000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*3);
	opchar = "or";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xa000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "xor";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xb000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*5);
	opchar = "trap";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xd000;
	hashmap_add(mapping,opchar,opcode);

	//RX instructions

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "lea";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*5);
	opchar = "load";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf100;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	opchar = "store";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf200;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*5);
	opchar = "jump";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf300;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*7);
	opchar = "jumpc0";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf400;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*7);
	opchar = "jumpc1";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf500;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	opchar = "jumpf";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xb000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	opchar = "jumpt";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf700;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	opchar = "jal";
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf800;
	hashmap_add(mapping,opchar,opcode);

	//EX instructions will be implemented later


	hashmap_print(mapping);
	return mapping;
}