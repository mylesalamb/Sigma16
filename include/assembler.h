#ifndef ASSEMBLER_GUARD
#define ASSEMBLER_GUARD
#include<stdint.h>
#include"map.h"

	typedef struct rxarg{
		uint16_t reg;
		uint16_t mem;
		bool resolved;
	} rxarg_t;

	int getnextreg(char * buffer, int * left);
	uint16_t * getobjcode(FILE * fp);
	int strhash(void * arg);
	int strcomp(void * arga, void * argb );
	int getnexttoken(char * str, int i);
	uint16_t getrrrargs(char * buffer,int left);
	int isvalidid(char * str,int left,int right);
	void strslice(char * src, char * dest, int left, int right);
	static hashmap_t * opmapinit();
#endif