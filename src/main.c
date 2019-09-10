/*
Author: Myles Lamb

Arguement parser and entry point for the Sigma16 emulator
receives a singular arguement from teh user either flagged with -f or by itself


*/
#include<stdio.h>
#include<unistd.h>
#include<string.h>


#include"parser.h"

int main(int argc, char * const argv[]){
	char flag;
	opterr = 0; //supress getopt err
	char * filename = NULL;

	while((flag = getopt(argc,argv,"f:")) != -1 ){
		
		switch(flag){
			
			case 'f':
				filename = optarg;
				break;

			case '?':
				goto err;
				break;
			}
		}

	

	//the user has not flagged filename, and weve expended argv
	if( (filename==NULL)){

		if(optind == argc)
			goto err;
		else
			filename = argv[optind];

	}

	FILE * fp = fopen(filename,"r");

	if(fp == NULL)
		goto err;

	printf("Starting Parser...\n");
	getobjcode(fp);

	return 0;

	err:
		fprintf(stderr,"Invalid args\nUSAGE: Sigma16 -f [Infile]\n"
			"check format and that file exists\n");
		return 1;
}
