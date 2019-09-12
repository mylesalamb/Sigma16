/*
Author: Myles Lamb

Assembler related utilities for the sigma16 emulator
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

static hashmap_t * opmapinit();

int line = 0;

//used at the end of assembly to resolve references to labels
typedef struct unresolved_label_node{
	char * label;
	uint16_t instruction;
	struct unresolved_label_node * next;
} unlabel_t ;



/*
parses a sigma16 asm file returning an array of
16bit integers resolving references to labels

returns: a 2^16 array of 16bit opcodes corresponding to the assembler input


*/
uint16_t * getobjcode(FILE * fp){

	unlabel_t * head = NULL;
	hashmap_t * codebook = opmapinit();
	hashmap_t * labelresolver = hashmap_init(20,&strhash,&strcomp);
	char * buffer = (char*)malloc(sizeof(char)*BUFFSIZE);
	uint16_t * mem = (uint16_t*)malloc(sizeof(uint16_t)*MEMSIZE);
	char * op;
	int ip = 0;

	while(fgets(buffer,BUFFSIZE,fp) != NULL){

		int lcursor = 0;
		int rcursor = 0;
		line++;

		//blank or comment line
		lcursor = getnexttoken(buffer,lcursor);
		if(lcursor==-1)
			continue;

		//there is a label on this line
		if(lcursor==0){

			rcursor = lcursor;

			while( !isspace(buffer[rcursor]))
				rcursor++;
			

			if(!isvalidid(buffer,lcursor,rcursor)){
				errhandler("Label is not valid");
			}
			
			//store in our codebook to resolve references later
		 	int len = rcursor - lcursor;
			char * label = (char *)malloc(sizeof(char)*(len+1));
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

		while(isalpha(buffer[rcursor]))
			rcursor++;

		int len = rcursor - lcursor;
		op = (char*)malloc(sizeof(char)*(len+1));
		

		strslice(buffer,op,lcursor,rcursor);
		
		if(strcmp(op,"data") == 0){
			lcursor = getnexttoken(buffer,rcursor);
			mem[ip++] = getlit(buffer,lcursor);
			continue;

		}

		uint16_t * opcode = (uint16_t*)hashmap_get(codebook,op);
		
		if(opcode == NULL){
			errhandler("Instruction is not valid");
		}
		

		uint16_t opval = *opcode;

		lcursor = getnexttoken(buffer,rcursor);
		if(lcursor==-1){
			errhandler("No/Bad operands");

		}

		//get args and add them into the 16bit word

		if(isrrr(*opcode)){
			uint16_t args = getrrrargs(buffer,&lcursor);
			opval ^= args; 
			mem[ip++] = opval;

			

			}
		else{
			rxarg_t args = getrxargs(buffer,&lcursor);
			mem[ip++] = args.reg ^ opval;
			if(args.label != NULL ){

				//init new node for linked list
				unlabel_t * ref = (unlabel_t *)malloc(sizeof(unlabel_t));
				ref->label = args.label;
				ref->instruction = ip++; //2 words so instruction is one after ops
				ref->next = head;
				head = ref;
			}
			else{
				//opcode and registers
				mem[ip++] = args.mem; //memory address
			}
		}

		//check if there is anything left on the line
		lcursor = getnexttoken(buffer,lcursor);
			if(lcursor!=-1)
				errhandler("Unparseable line after instruction,"
				" did you mean to comment this?");



	}


	//label resolution

	unlabel_t * next;
	printf("got here\n");

	while(head!=NULL){

		uint16_t * ref = (uint16_t *)hashmap_get(labelresolver,head->label);
		if(ref == NULL)
			errhandler("could not resolve a label ");

		mem[head->instruction] = *ref;

		next = head->next;

		free(head->label);
		free(head);

		head=next;

	}


	//print the parsed instructions for debugging purposes
	for(int i =0; i < ip;i++)
		printf("%.4x\n",mem[i] );

	return mem;
}

/*
resolves the operands in a rx instruction returning whether any
labels used were resolved or not

moves the cursor as far as it succesfully parses, exits the program on failure

*/
rxarg_t getrxargs(char * buffer, int * left){
	rxarg_t ret = {0x0000,0x0000,NULL};

	uint16_t registers = 0x0000;
	uint16_t memory = 0x0000;

	//shift and xor into correct form
	int reg = getnextreg(buffer,left);
	registers = registers << 4;	  
	registers ^= reg;

	if(buffer[*left] != ','){
		printf("bad args\n");
		exit(0);
	}

	(*left)++;
	int right = *left;
	while(buffer[right] != '[' && buffer[right] != '\0')
		right++;

	if(buffer[right] == '\0'){
		errhandler("Bad RX operands");
	}

	if(isvalidid(buffer,*left,right)){
		char * id = (char *)malloc(sizeof(char)*(right-(*left) + 1 ));
		strslice(buffer,id,*left,right);
		printf("%s\n",id );
		ret.label = id;


	}
	else{
		printf("not label\n");
		memory = getlit(buffer,*left);
		//ret.resolved = true;
	}

	*left = ++right;

	//shift into the form 0(ra)(rb)0 mem

	reg = getnextreg(buffer,left);
	registers = registers << 4;	  
	registers ^= reg;
	registers = registers << 4;

	(*left)++ ; // skip closing bracket of rx instruction

	return ret;

}



uint16_t getrrrargs(char * buffer,int * left){

	uint16_t args = 0;

	for(int i = 0; i < 3; i++){
		
		int reg = getnextreg(buffer,left);
		args = args << 4;
		args = args ^ reg;
		(*left)++; //skip comma
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

	if(buffer[*left] != 'r' && buffer[*left] != 'R'){
		printf("%c\n",buffer[*left] );
		errhandler("invalid operand form");
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
		(*left)++;
	}

	if(reg > 15){
		errhandler("Register out of bounds");
	}

	return reg;
}
/*
utility for reading numeric literals in the form
cursor is assumed to be pointing to the first character of
the literal
1234 : decimal
#1010101 : binary
$ffff : hex

*/
uint16_t getlit(char * buffer, int cursor){
	int base;
	uint16_t ret=0;
	long check=0;
	int right;

	if(isdigit(buffer[cursor])){
		base = 10;
		right = cursor;
		while(isdigit(buffer[right]))
			right++;
	}
	else if(buffer[cursor] == '$'){
		base=16;
		(cursor)++;
		right=cursor;

		while(isxdigit(buffer[right]))
			right++;

	}
	else if(buffer[cursor]== '#'){
		base = 2;
		(cursor++);
		right = cursor;
		while(buffer[right] == '1' || buffer[right] == '0')
			right++;
	}
	else{
		errhandler("Unrecognised literal");
	}

		char str[right-cursor];
		strslice(buffer,str,cursor,right);
		check = strtol(str,NULL,base);

	if(check > UINT16_MAX)
			errhandler("Literal value overflow");
	ret = check;

	return ret;

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

dest should be right-left+1 chars long for null terminating
*/
void strslice(char * src, char * dest, int left, int right){

	for(int i = left,j=0; i < right; i++,j++)
		dest[j] = src[i];

	dest[right-left] =  '\0';


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

void errhandler(char * errstring){
	fprintf(stderr, "ERR(%d):%s\n",line,errstring);
	exit(0);
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