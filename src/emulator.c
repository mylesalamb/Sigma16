


#include<stdlib.h>
#include<stdio.h>
#include<stdint.h>
#include<ncurses.h>

#include"assembler.h"

/*
Entrypoint for the emulator


*/
void start(FILE * fp){


	uint16_t * memory = getobjcode(fp); 
	printf("Memory initialised...\n");
	uint16_t regfile[16],op,mem;

	initui();

	while(getch() != 'q')
	; 
	
	endui();






}