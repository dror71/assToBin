#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NO_ERRORS 0
#define LONG_LINE 1
#define LABEL_STARTS_WITH_DIGIT 2
#define ILEAGEL_FIRST_CHAR 3
#define SAME_NAME_LABEL 4
#define ILEAGEL_COMMA_IN_DIRECTIVE_LINE 5
#define ILEAGEL_ASCIZ_COMMAND 6
#define ILEAGEL_LABEL 7
#define ILEAGEL_REGISTER 8
#define NUMBER_OF_REGISTERS 9
#define LABEL_NOT_FOUND 10
#define ASSAMBLER_ERROR 11
#define BRANCH_WITH_EXTERNAL_LABEL 12

#define MAX_LABEL 32
#define MAX 80
#define ACTIONS_NUM 27
#define CHAR_BIT 8 

#define DATA 'd'
#define CODE 'c'
#define EXTERN 'e'

struct keywords {
	char *word;
};

typedef struct actions{
	char *word;
	int op_code;
	int func_code;
	char type;
}actions;

typedef struct nlist {  /*symbol table*/
	struct nlist *next;
	char symbol[MAX_LABEL];
	char atributes[MAX_LABEL];
	int value;
}nlist;

typedef struct DataImage {
	struct DataImage *next;
	int address;
	char machine_code[33];
	int is_data;
	int dc;
	int ic;
}DataImage;

typedef struct commandExternal {
	int address;
	char name[MAX_LABEL];
	struct commandExternal *next;

}commandExternal;

extern struct actions actions_struct[27];



/*first pass*/

int assemble(char *);
void showError(int);
int ignore(char *);
int readLine(char *, int);
int isLabel(char *, int);
char * skipspace(char *);
char* getWord(char *, int *);
char* moveToNextWord(char *, int);
int isDirective(char *);
int addToSymbolTable(char *, char, int);
int addDataStorageDirective(char *,char *);
int handleDh(char *);
int handleDb(char *);
int handleDw(char *);
int handleAsciz(char *);
int addExternDirective(char *, char *);
int handleAction(char *, char *);
int processAction(char *, actions);
int isLeagelRegister(char *);
int processRtype(char *, actions);
int copyCommand(char *, actions);
int processItype(char *, actions);
int processJtype(char *, actions);
int processBranching(char *, actions);
int processArithmetic(char *, actions);
void updateAdressSymbolTabel();
void updateAdressDataImage();
int processLoadSave(char *, actions);
int isLeagelDirective(char *);


/*second pass*/
int readLineSecondPass(char*);
int secondPass(char *);
int handleActionSecondPass(char *, char *);
int processActionSecondPass(char *, actions);
char * skipComma(char *);
void freeCommandExt();
int processItypeSecondPass(char *, actions);
int processJtypeSecondPass(char *, actions);

/*output generator*/
void createOutPutFiles(char *);
char *removeExtension(char *);
char * addExtension(char *, int);
void createObOutput(char *);
char * intToString(int);
char *binaryByteToHexa(char *);
void createEntOutput(char *);
void createExtOutput(char *);
int twoPower(int);



/*symbol table*/
nlist * createTable(char *, char *, int);
nlist * addNewEntry(char *, char *, int);
nlist * findLabel(char *);
void printSymboleTable(); 
void freeSymbolTable();

/*data iamge*/
DataImage * createImageTable(int , char *, int, int, int);
DataImage * addNewImageEntry(int , char *, int , int , int);
void add_machine_code(DataImage *, char *);
void printDataImage(); 
char * int2bin(int , int);
DataImage * findLineByIc(int);
void freeDataImage();



/*Global variables*/
int ic, dc, ac, icf, dcf;
int error; 

extern nlist *symbols_table;
extern DataImage *data_image;
extern commandExternal *command_external;

