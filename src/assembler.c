/*
Author: Myles Lamb

Parser related utilities for the sigma16 emulator
translates asm into opcodes for easier manipulation by the emulator
so we can retain the 'code as data' behaviour see (strange.asm.txt) 
*/

#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<stdbool.h>
#include"map.h"
#include"assembler.h"

#define BUFFSIZE 200
#define MEMSIZE 65536
#define OP_BUCKETS 10
#define isrrr(x) ( (0xf000 & x) <= 0xe000)
#define isrx(x) ( (0xf000) & x == 0xf000 )

/*
parses a sigma16 asm file returning an array of
16bit integers resolving references to labels

returns: a 2^16 array of 16bit opcodes corresponding to the assembler input


*/
uint16_t * getobjcode(FILE * fp){

	hashmap_t * codebook = opmapinit();
	hashmap_t * labelresolver = hashmap_init(20,&strhash,&strcomp);

	char * buffer = (char*)malloc(sizeof(char)*BUFFSIZE);
	uint16_t * mem = (uint16_t*)malloc(sizeof(uint16_t)*MEMSIZE);
	
	char * op;
	char * errstring;
	int line = 0;
	int ip = 0;

	while(fgets(buffer,BUFFSIZE,fp) != NULL){

		int lcursor = 0;
		int rcursor = 0;
		line++;

		//blank or comment line
		lcursor = getnexttoken(buffer,lcursor);
		printf("%d\n",lcursor );
		if(lcursor==-1)
			continue;

		//there is a label on this line
		if(lcursor==0){

			rcursor = lcursor;

			while( !isspace(buffer[rcursor] )){
				rcursor++;
			}

			if(!isvalidid(buffer,lcursor,rcursor)){
				errstring = "Label is not valid";
				goto err;
			}
			
			//store in our codebook to resolve references later
		 	int len = rcursor - lcursor;
			char * label = (char *)malloc(sizeof(char)*len+1);
			strslice(buffer,label,lcursor,rcursor);
			uint16_t * ref = (uint16_t *)malloc(sizeof(uint16_t));
			*ref = ip;
			hashmap_add(labelresolver,label,ref);
			
			//check if there is anymore stuff on the line
			lcursor= rcursor;
			lcursor = getnexttoken(buffer,lcursor);
			if(lcursor==-1)
				continue;

		}

		rcursor = lcursor;

		while(isalpha(buffer[rcursor++]))
			;

		int len = rcursor - lcursor;
		op = (char*)malloc(sizeof(char)*len);
		

		strslice(buffer,op,lcursor,rcursor);
		uint16_t * opcode = (uint16_t*)hashmap_get(codebook,op);
		if(opcode == NULL){
			errstring = "instrction is not valid";
			goto err;
		}

		uint16_t opval = *opcode;

		lcursor = getnexttoken(buffer,rcursor);
		if(lcursor==-1){
			errstring = "No/bad operands";
			goto err;

		}

		//get args and add them into the 16bit word

		if(isrrr(*opcode)){
			uint16_t args = getrrrargs(buffer,lcursor);
			opval ^= args; 
			printf("%.4x\n",opval );
			mem[ip++] = opval;
			}
		else{
			printf("instruction is rx i dont know what to do\n");
		}



	}

	return mem;



	err:
		fprintf(stderr, "(%d) ERR: %s\n%s",line,errstring,buffer);
		exit(1);

}


rxarg_t getrxargs(char * buffer, int left){
	rxarg_t ret = {0x0000,0x0000,false};

	int reg = getnextreg(buffer,&left);
	ret.reg = ret.reg << 4
	ret.reg ^= reg;

	if(buffer[left] != ','){
		printf("bad args\n");
		exit(0);
	}

	left++;
	int right = left;
	while(buffer[right] != '[' && buffer[right] != '\0')
		right++;

	if(buffer[right] == '\0'){
		printf("bad args\n");
		exit(0);
	}

	if(isvalidid(buffer,left,right))


	

	ret.reg = ret.reg << 4
	return ret;

}



uint16_t getrrrargs(char * buffer,int left){

	uint16_t args = 0;

	for(int i = 0; i < 3; i++){
		
		int reg = getnextreg(buffer,&left);
		args = args << 4;
		args = args ^ reg;
		left++; //skip comma
	}

	return args;

}


/*
gets the next register in the form 'rxx | Rxx'
and returns as an integer, modifies the pointer in the
buffer up to the first character that is not an integer
exits if the register is not of the correct form
*/
int getnextreg(char * buffer, int * left){

	if(buffer[*left] != 'r' && buffer[*left] == 'R'){
		printf("bad args in reg parser\n");
		exit(0);
	}

	(*left)++; //skip r

	int reg = 0;

	if(isdigit(buffer[*left])){
		reg += buffer[*left] - '0';
	}

	(*left)++;

	if(isdigit(buffer[*left])){
		reg *= 10;
		reg += buffer[*left] - '0';
		(*left++);
	}

	if(reg > 15){
			printf("bad args, not register number oob");
			exit(0);
	}

	return reg;
}


/*
Utility to trim of whitespace from between parts of line
returns -1 if end of line or comment is reached
*/
int getnexttoken(char * str, int i){

	while(isspace(str[i]))
		i++; //trim of whitespace from the start of i

	//have we reached the end of a line?
	return (str[i]=='\n' || str[i]==';'||str[i]=='\0') ? -1 : i;
		
}


/*
Copies characters from src to dest within the range left - right
same behaviour as python
dest = src[left:right]
*/
void strslice(char * src, char * dest, int left, int right){

	for(int i = left,j=0; i < right; i++,j++)
		dest[j] = src[i];

	dest[right-left-1] =  '\0';


}




/*
Returns whether the identifier str[left:right] is valid
(1st char is a letter and may contain letters digits or underscores)
*/
int isvalidid(char * str,int left,int right){

	if(!isalpha(str[left]))
		return 0;

	for(int i = left + 1; i<right;i++){

		if(!isalpha(str[i]) && !isdigit(str[i]) && str[i] != '_' )
			return 0;


	}

	return 1;
}

/*
simple string hashing function
*/
int strhash(void * arg){
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

	hashmap_t * mapping = hashmap_init(OP_BUCKETS,&strhash,&strcomp);

	int status;

	//creating the mapping

	//RRR instructions

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"add");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x0000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"sub");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x1000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"mul");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x2000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"div");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x3000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	strcpy(opchar,"cmplt");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x4000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	strcpy(opchar,"cmpeq");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x5000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	strcpy(opchar,"cmpgt");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x6000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"cmp");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x7000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"inv");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x8000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"and");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0x9000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*3);
	strcpy(opchar,"or");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xa000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"xor");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xb000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*5);
	strcpy(opchar,"trap");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xd000;
	hashmap_add(mapping,opchar,opcode);

	//RX instructions

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"lea");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf000;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*5);
	strcpy(opchar,"load");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf001;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	strcpy(opchar,"store");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf002;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*5);
	strcpy(opchar,"jump");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf003;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*7);
	strcpy(opchar,"jumpco");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf004;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*7);
	strcpy(opchar,"jumpc1");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf005;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	strcpy(opchar,"jumpf");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf006;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*6);
	strcpy(opchar,"jumpt");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf007;
	hashmap_add(mapping,opchar,opcode);

	opchar = (char *)malloc(sizeof(char)*4);
	strcpy(opchar,"jal");
	opcode =(uint16_t*)malloc(sizeof(uint16_t));
	*opcode = 0xf008;
	hashmap_add(mapping,opchar,opcode);

	//EX instructions will be implemented later

	return mapping;
}