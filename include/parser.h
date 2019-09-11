#ifndef PARSER_GUARD
#define PARSER_GUARD
#include<stdint.h>
#include"map.h"

uint16_t * getobjcode(FILE * fp);
int strhash(void * arg);
int strcomp(void * arga, void * argb );
int getnexttoken(char * str, int i);
int isvalidid(char * str,int left,int right);
void strslice(char * src, char * dest, int left, int right);
static hashmap_t * opmapinit();
#endif