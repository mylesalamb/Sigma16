#ifndef ASSEMBLER_GUARD
#define ASSEMBLER_GUARD
#include<stdint.h>
#include<stdbool.h>
#include"map.h"

#define isrrr(x) ( (0xf000 & x) <= 0xe000)
#define isrx(x) ( (0xf000) & x == 0xf000 )

	typedef struct rxarg{
		uint16_t reg;
		uint16_t mem;
		char * label; //handle strange case where label is addr 0
	} rxarg_t;

	rxarg_t getrxargs(char * buffer, int * left);

	uint16_t getlit(char * buffer, int  cursor);
	void errhandler(char * errstring);
	int getnextreg(char * buffer, int * left);
	uint16_t * getobjcode(FILE * fp);
	int strhash(void * arg);
	int strcomp(void * arga, void * argb );
	int getnexttoken(char * str, int i);
	uint16_t getrrrargs(char * buffer,int * left);
	int isvalidid(char * str,int left,int right);
	void strslice(char * src, char * dest, int left, int right);
#endif