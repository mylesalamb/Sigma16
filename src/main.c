/*
Author: Myles Lamb

Arguement parser and entry point for the Sigma16 emulator
receives a singular arguement from teh user either flagged with -f or by itself


*/
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include"parser.h"

int main(int argc, char * const argv[])
{
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

		}

	}

	//the user has not flagged filename, and weve expended argv
	if( (filename==NULL && optind == argc) )
		goto err;

	filename = argv[optind];
	FILE * fp = fopen("filename","r");

	if(fp == NULL)
		goto err;

	


	return 0;

	err:
		fprintf(stderr,"Invalid args\nUSAGE: Sigma16 -f [Infile]\n"
			"check format and that file exists");
		return 1;

}