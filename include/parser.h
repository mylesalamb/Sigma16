#ifndef PARSER_GUARD
#define PARSER_GUARD
#include<stdint.h>

uint16_t * getobjcode(FILE * fp);
int strhash(void * arg);
int strcomp(void * arga, void * argb );
#endif