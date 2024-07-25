#define _CRT_SECURE_NO_WARNINGS
#define DIRECTIVE_NUM 6
#define BYTE 8
#define HALF_WORD 16
#define WORD 32

#include <stdio.h>
#include "asb.h"
#include <limits.h>
#include <stdlib.h>

struct keywords directive[] = {
  {"dh"},{"dw"},{"db"},{"asciz"},
  {"entry"},{"extern"}
};

struct actions actions_struct[27] = {
  {"add",0,1,'r'},{"sub",0,2,'r'},{"and",0,3,'r'},{"or",0,4,'r'}, {"nor",0,5,'r'},
{"move",1,1,'r'},{"mvhi",1,2,'r'}, {"mvlo",1,3,'r'},{"addi",10,0,'i'},{"subi",11,0,'i'},{"andi",12,0,'i'},{"ori",13,0,'i'},
{"nori",14,0,'i'},{"bne",15,0,'i'},{"beq",16,0,'i'},{"blt",17,0,'i'}, {"bgt",18,0,'i'},{"lb",19,0,'i'},
{"sb",20,0,'i'},{"lw",21,0,'i'},{"sw",22,0,'i'},{"lh",23,0,'i'},{"sh",24,0,'i'},{"jmp",30,0,'j'},
{"la",31,0,'j'},{"call",32,0,'j'},{"stop",63,0,'j'}
};


int line_counter = 1;



int assemble(char *file_name) {
	char line[MAX];
	FILE *file_ptr;
	file_ptr = fopen(file_name, "r");
	if (!file_ptr) {
		printf("Cannot open file %s", file_name);
		return 0;
	}

	ic = 100;
	dc = 0;

	while (1) {

		if (fgets(line, MAX, file_ptr) == NULL)
			break;		/* Finish reading file, exiting loop. */

		if (!ignore(line)) { /*if the line is not empty or a comment*/
			if (strlen(line) > MAX - 2) {
				error = LONG_LINE;
				showError(line_counter);
				fgets(line, MAX, file_ptr); /*finish the long line*/
			}
			else {
				if (readLine(line, line_counter) == 0) {
					printf("assembly stoped due to an error\n");
					break;
				}
			}
		}
		line_counter++;
	}

	icf = ic;
	dcf = dc;
	updateAdressSymbolTabel();
	updateAdressDataImage();
	fclose(file_ptr);
	return secondPass(file_name);
}


int readLine(char *line, int line_num) {
	int symbol_flag = 0;
	char label[MAX_LABEL];
	char *current_word;
	int curr_word_leng = 0;
	int dir_type;
	if (isspace(*line) != 0) {
		line = skipspace(line);
	}
	/* first character can either be a dot or a letter. */
	if (isalpha(*line) == 0 && *line != '.') {
		error = ILEAGEL_FIRST_CHAR;
		showError(line_num);
		return 0;
	}

	current_word = getWord(line, &curr_word_leng);
	line = moveToNextWord(line, curr_word_leng);
	
	if ((isLabel(current_word,line_num)) != 0 && line!=NULL) {
		symbol_flag = 1;
		strcpy(label, current_word);
		label[strlen(current_word)-1] = '\0';
		free(current_word);
		current_word = getWord(line, &curr_word_leng);
		line = moveToNextWord(line, curr_word_leng);
		line = skipspace(line);
	}

	if ((dir_type = isDirective(current_word)) != 0) {
		if (symbol_flag == 1 && dir_type == 1) {
			addToSymbolTable(label, DATA, dc);
			symbol_flag = 0;
		}
		if (dir_type == 1) {
			if (addDataStorageDirective(line, current_word) == 1) {
				return 1;
			}
			else {
				return 0;
			}
		}
		if (dir_type == 2) {
			if (addExternDirective(line, current_word) == 1) {
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	if (symbol_flag == 1) {
		if (addNewEntry(label, "code", ic) == NULL) {
			return 0;
		}
	}

	if (handleAction(line, current_word) == 0) {
		return 0;
	}
	ic += 4;

	free(current_word);
	return 1;
}

/*handle actions checks if its a leagel action if not returns zero
if its leagel send to another function to contuie processing*/
int handleAction(char *line, char *action) {
	int i;
	for (i = 0; i < ACTIONS_NUM; i++) {
		if (strcmp(actions_struct[i].word, action) == 0) {
			return processAction(line, actions_struct[i]);
		}
	}
	return 0;
}

/*identifys what type is the actions and send it to the propper function to continue processing*/
int processAction(char *line, actions command) {
	switch (command.type) {
	case 'r':
		return processRtype(line, command);
		break;
	case 'i':
		return processItype(line, command);
		break;
	case 'j':
		return processJtype(line, command);
		break;
	default:
		return 1;
		break;
	}
}

int processJtype(char *line, actions command) {
	char *temp;
	int i = 0;
	char regis[3];
	int element;
	char *code = (char*)malloc(33);
	if (command.op_code == 30) {
		if (isLeagelRegister(line) == 1) {
			while (isspace(*line) == 0 && *line !='\n' && *line != '\0') {
				if (*line == '$') {
					line++;
				}
				regis[i++] = *(line);
				line++;
			}
			regis[i] = '\0';
			element = atoi(regis);
			temp = int2bin(command.op_code,6);
			strcpy(code, temp);
			free(temp);
			temp = int2bin(1, 1);
			strcat(code, temp);
			free(temp);
			temp = int2bin(0, 20);
			strcat(code,temp );
			free(temp);
			temp = int2bin(element, 5);
			strcat(code,temp );
			data_image = addNewImageEntry(ic, code, 0, dc, ic);
			free(code);
			return 1;
		}
	}
	temp = int2bin(command.op_code, 6);
	strcpy(code, temp);
	free(temp);
	temp = int2bin(0, 1);
	strcat(code,temp);
	free(temp);
	if (command.op_code == 63) {
		temp = int2bin(0, 25);
		strcat(code, temp);
		free(temp);
	}
	data_image = addNewImageEntry(ic, code, 0, dc, ic);
	free(code);
	
	return 1;
}
int processItype(char *line, actions command) {
	if (command.op_code < 15) {
		return processArithmetic(line, command);
	}
	else if (command.op_code < 19) {
		return processBranching(line, command);
	}
	else if (command.op_code < 30) {
		return processLoadSave(line, command);
	}
	else {
		return 1;
	}
}

int processLoadSave(char *line, actions command) {
	char *temp;
	char value[10];
	int i = 0;
	int argument_counter = 0;
	int j = 0;
	int elements[3];
	char *code;
	code = (char*)malloc(33);
	while (*line != '\0' && *line != '\n') {
		if (isLeagelRegister(line) == 0 && argument_counter != 1) {
			error = ILEAGEL_REGISTER;
			showError(line_counter);
			return 0;
		}
		while (*line != ',' && *line != '\0' && *line != '\n' && *line != ' ') {
			if (*line == '$') {
				line++;
			}
			value[i++] = *line;
			line++;
		}
		value[i] = '\0';
		elements[j++] = atoi(value);
		argument_counter++;
		i = 0;
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
		if (*line == ',') { line++; }
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
	}
	if (argument_counter != 3) {
		error = NUMBER_OF_REGISTERS;
		showError(line_counter);
		return 0;
	}

	temp = int2bin(command.op_code, 6);
	strcpy(code, temp);
	free(temp);
	temp = int2bin(elements[0], 5);
	strcat(code,temp );
	free(temp);
	temp = int2bin(elements[2], 5);
	strcat(code,temp );
	free(temp);
	temp = int2bin(elements[1], 16);
	strcat(code,temp );
	free(temp);
	data_image = addNewImageEntry(ic, code, 0, dc, ic);
	free(code);

	return 1;
}


int processArithmetic(char *line, actions command){
	char *temp;
	char value[10];
	int i = 0;
	int argument_counter = 0;
	int j = 0;
	int elements[3];
	char *code;
	code = (char*)malloc(33);
	while (*line != '\0' && *line != '\n') {
		if (isLeagelRegister(line) == 0 && argument_counter != 1) {
			error = ILEAGEL_REGISTER;
			showError(line_counter);
			return 0;
		}
		while (*line != ',' && *line != '\0' && *line != '\n' && *line != ' ') {
			if (*line == '$') {
				line++;
			}
			value[i++] = *line;
			line++;
		}
		value[i] = '\0';
		elements[j++] = atoi(value);
		argument_counter++;
		i = 0;
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
		if (*line == ',') { line++; }
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
	}
	if (argument_counter != 3) {
		error = NUMBER_OF_REGISTERS;
		showError(line_counter);
		return 0;
	}
	temp = int2bin(command.op_code, 6);
	strcpy(code, temp);
	free(temp);
	temp = int2bin(elements[0], 5);
	strcat(code,temp );
	free(temp);
	temp = int2bin(elements[2], 5);
	strcat(code, temp);
	free(temp);
	temp = int2bin(elements[1], 16);
	strcat(code,temp );
	free(temp);
	data_image = addNewImageEntry(ic, code, 0, dc, ic);	

	free(code);
	return 1;
}

int processBranching(char *line, actions command) {
	char *temp;
	int stop = 0;
	char value[4];
	int i = 0;
	int register_counter = 0;
	int registers[2];
	int j = 0;
	char *code;
	code = (char*)malloc(33);
	while (*line != '\0' && *line != '\n') {
		if (isLeagelRegister(line) == 0) {
			error = ILEAGEL_REGISTER;
			showError(line_counter);
			return 0;
		}
		while (*line != ',' && *line != '\0' && *line != '\n' && *line != ' ' && stop == 0) {
			if (*line == '$') {
				line++;
			}
			value[i++] = *line;
			line++;
		}
		value[i] = '\0';
		registers[j++] = atoi(value);
		register_counter++;
		i = 0;
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
		if (*line == ',' ) {
			if (register_counter < 2)
				line++;
			else {
				stop = 1;
				break;
			}
		}
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
	}
	if (register_counter != 2) {
		error = NUMBER_OF_REGISTERS;
		showError(line_counter);
		return 0;
	}
	
	temp = int2bin(command.op_code, 6);
	strcpy(code, temp);
	free(temp);
	temp = int2bin(registers[0], 5);
	strcat(code,temp );
	free(temp);
	temp = int2bin(registers[1], 5);
	strcat(code,temp);
	free(temp);
	data_image = addNewImageEntry(ic, code, 0, dc, ic);

	free(code);
	return 1;
}


int processRtype(char *line, actions command) {
	char value[4];
	int i = 0;
	int register_counter = 0;
	int registers[3];
	int j = 0;
	char *code;
	char *temp;
	code = (char*)malloc(33);
	if (command.op_code == 0) {
		while (*line != '\0' && *line != '\n') {
			if (isLeagelRegister(line) == 0) {
				error = ILEAGEL_REGISTER;
				showError(line_counter);
				return 0;
			}
			while (*line != ',' && *line != '\0' && *line != '\n' && *line != ' ') {
				if (*line == '$') {
					line++;
				}
				value[i++] = *line;
				line++;
			}
			value[i] = '\0';
			registers[j++] = atoi(value);
			register_counter++;
			i = 0;
			if (*line != '\0' && *line != '\n') {
				line = skipspace(line);
			}
			if (*line == ',') { line++; }
			if (*line != '\0' && *line != '\n') {
				line = skipspace(line);
			}
		}
		if (register_counter != 3) {
			error = NUMBER_OF_REGISTERS;
			showError(line_counter);
			return 0;
		}

		temp = int2bin(command.op_code,6);
		strcpy(code, temp);
		free(temp);
		temp = int2bin(registers[0], 5);
		strcat(code, temp);
		free(temp);
		temp = int2bin(registers[1], 5);
		strcat(code,temp);
		free(temp);
		temp = int2bin(registers[2], 5);
		strcat(code, temp);
		free(temp);
		temp = int2bin(command.func_code, 5);
		strcat(code, temp);
		free(temp);
		temp = int2bin(0, 6);
		strcat(code,temp );
		free(temp);
		data_image = addNewImageEntry(ic,code,0, dc, ic);
		free(code);
	}
	else {
		return copyCommand(line, command);
	}
	return 1;
}

int copyCommand(char *line, actions command) {
	char value[4];
	int i = 0;
	int register_counter = 0;
	int registers[2];
	int j = 0;
	char *code;
	char *temp;
	code = (char*)malloc(33);
	while (*line != '\0' && *line != '\n') {
		if (isLeagelRegister(line) == 0) {
			error = ILEAGEL_REGISTER;
			showError(line_counter);
			return 0;
		}
		while (*line != ',' && *line != '\0' && *line != '\n' && *line != ' ') {
			if (*line == '$') {
				line++;
			}
			value[i++] = *line;
			line++;
			}
		value[i] = '\0';
		registers[j++] = atoi(value);
		register_counter++;
		i = 0;
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
		if (*line == ',') { line++; }
		if (*line != '\0' && *line != '\n') {
			line = skipspace(line);
		}
	}
	if (register_counter != 2) {
		error = NUMBER_OF_REGISTERS;
		showError(line_counter);
		return 0;
	}
	temp = int2bin(command.op_code, 6);
	strcpy(code, temp);
	free(temp);
	temp = int2bin(registers[0], 5);
	strcat(code,temp );
	free(temp);
	temp = int2bin(0, 5);
	strcat(code,temp );
	free(temp);
	temp = int2bin(registers[1], 5);
	strcat(code, temp);
	free(temp);
	temp = int2bin(command.func_code, 5);
	strcat(code,temp);
	free(temp);
	temp = int2bin(0, 6);
	strcat(code, temp);
	free(temp);
	data_image = addNewImageEntry(ic, code, 0, dc, ic);
	free(code);

	return 1;
}

/*return 1 if the next word is a leagel register 0 otherwise*/
int isLeagelRegister(char *line) {
	char value[MAX_LABEL];
	int i = 0;
	if (*line != '$') {
		return 0;
	}
	while (*line != ',' && *line != '\0' && *line != '\n') {
		value[i++] = *line;
		line++;
	}
	value[i] = '\0';
	if (atoi(value) > 31 || atoi(value) < 0) {
		return 0;
	}
	return 1;
}

/*if found entry ignors and returns 1
if found exten adds to symbol table with type extern */
int addExternDirective(char *line, char *directive) {
	char label[MAX_LABEL];
	int i = 0;
	if (strcmp(directive, ".entry") == 0) {
		return 1;
	}
	if (!isalpha(*line)) {
		error = ILEAGEL_LABEL;
		showError(line_counter);
		return 0;
	}
	while (*line != '\n' && *line != '\0' && isspace(*line) == 0) {
		label[i++] = *line;
		line++;
	}
	label[i] = '\0';
	if (addToSymbolTable(label, EXTERN, 0) == 0) {
		return 0;
	}
	return 1;
}

/*adds directive data values to the data_image
return 1 if succeful 0 otherwise*/
int addDataStorageDirective(char *line, char *directive){
	
	directive++; /*skips the dot*/

	if (strcmp(directive, "asciz") == 0) {
		return handleAsciz(line);
	}
	else if (strcmp(directive, "db") == 0) {
		return handleDb(line); 
	}
	else if (strcmp(directive, "dh") == 0) {
		return handleDh(line);
	}
	else if (strcmp(directive, "dw") == 0) {
		return handleDw(line);
	}
	else {
		return 1;
	}
}

int handleAsciz(char *line) {
	char *temp;
	char value;
	int dc_incrementation = 1;
	if (*line != '"') {
		error = ILEAGEL_ASCIZ_COMMAND;
		showError(line_counter);
		return 0;
	}
	line++;
	while (*line != '"') {
		if (*line == '\0') {
			error = ILEAGEL_ASCIZ_COMMAND;
			showError(line_counter);
			return 0;
		}
		value = *line;
		temp = int2bin(value, BYTE);
		data_image = addNewImageEntry(0,temp ,1, dc, ic);
		free(temp);
		dc += dc_incrementation;
		line++;
	}
	dc++;
	temp = int2bin(0, BYTE);
	data_image = addNewImageEntry(0,temp , 1, dc, ic);
	free(temp);
	return 1;
}

int handleDb(char *line) {
	char *temp;
	int dc_incrementation = 1;
	char value[MAX];
	int i = 0;
	if (*line == ',') {
		error = ILEAGEL_COMMA_IN_DIRECTIVE_LINE;
		showError(line_counter);
		return 0;
	}
	while (*line != '\0' && *line != '\n') {
		while (*line != ',' && *line != '\0' && *line != '\n') {
			value[i++] = *line;
			line++;
		}
		value[i] = '\0';
		if (*(line + 1) == ',') {
			error = ILEAGEL_COMMA_IN_DIRECTIVE_LINE;
			showError(line_counter);
			return 0;
		}
		if (*line == ',') {
			line++;
		}
		temp = int2bin(atoi(value), BYTE);
		data_image = addNewImageEntry(0,temp, 1, dc, ic);
		free(temp);
		dc += dc_incrementation;
		line = skipspace(line);
		i = 0;
	}
	return 1;
}

int handleDh(char *line) {
	char *temp;
	int dc_incrementation = 2;
	char value[MAX];
	int i = 0;
	if (*line == ',') {
		error = ILEAGEL_COMMA_IN_DIRECTIVE_LINE;
		showError(line_counter);
		return 0;
	}
	while (*line != '\0' && *line != '\n') {
		while (*line != ',' && *line != '\0' && *line != '\n') {
			value[i++] = *line;
			line++;
		}
		value[i] = '\0';
		if (*(line + 1) == ',') {
			error = ILEAGEL_COMMA_IN_DIRECTIVE_LINE;
			showError(line_counter);
			return 0;
		}
		if (*line == ',') {
			line++;
		}
		temp = int2bin(atoi(value), HALF_WORD);
		data_image = addNewImageEntry(0, temp, 1, dc, ic);
		free(temp);
		dc += dc_incrementation;
		line = skipspace(line);
		i = 0;
	}
	return 1;
}

int handleDw(char *line) {
	char *temp;
	int dc_incrementation = 4;
	char value[MAX];
	int i = 0;
	if (*line == ',') {
		error = ILEAGEL_COMMA_IN_DIRECTIVE_LINE;
		showError(line_counter);
		return 0;
	}
	while (*line != '\0' && *line != '\n') {
		while (*line != ',' && *line != '\0' && *line != '\n') {
			value[i++] = *line;
			line++;
		}
		value[i] = '\0';
		if (*(line + 1) == ',') {
			error = ILEAGEL_COMMA_IN_DIRECTIVE_LINE;
			showError(line_counter);
			return 0;
		}
		if (*line == ',') {
			line++;
		}
		temp = int2bin(atoi(value), WORD);
		data_image = addNewImageEntry(0,temp, 1, dc, ic);
		free(temp);
		dc += dc_incrementation;
		line = skipspace(line);
		i = 0;
	}
	return 1;
}

/*adds a label to the symbol table*/
int addToSymbolTable(char *label, char type, int ic) {
	switch (type) {
	case DATA:
		if ((addNewEntry(label, "data", dc)) == NULL) {
			return 0;
		}
		break;
	case CODE:
		if ((addNewEntry(label, "code", ic)) == NULL) {
			return 0;
		}
		break;
	case EXTERN:
		if ((addNewEntry(label, "extern", ic)) == NULL) {
			return 0;
		}
		break;
	}
	if (error == SAME_NAME_LABEL) {
		showError(line_counter);
		return 0;
	}
	return 1;
}

/*checks if a word is Directive (starts with a dot '.'*/
int isDirective(char *word) {
	int temp;
	if (*word == '.') {
		temp = isLeagelDirective(++word);/*skips the dot*/
		return temp;
	}
	return 0;
}

/*return 0 if not leagel directive
return 1 if data storage directive
return 2 if entry or extern*/

int isLeagelDirective(char *word) {
	int i = 0;
	for (i = 0; i < DIRECTIVE_NUM; i++) {
		if (strcmp(directive[i].word, word) == 0) {
			if (strcmp(word, "entry") == 0 || strcmp(word, "extern") == 0) {
				return 2;
			}
			else {
				return 1;
			}
		}
	}
	return 0;
}
/*returns a poinetr to the next word in the line*/
char* moveToNextWord(char *line, int chars_to_advance) {
	line = line + chars_to_advance;
	line = skipspace(line);
	if (*line == '\n' || *line == '\0') {
		return NULL;
	}
	return line;
}

/*returns the current word
updates the value of word_length by refrence*/

char* getWord(char *line, int *word_length) {
	
	char *word;
	int i;

	word = (char*)malloc(MAX_LABEL);
	i = 0;
	(*word_length) = 0;
	while(line != NULL && isspace(*line) == 0 && *line != '\n' && *line != '\0' && *line){
		word[i++] = *line;
		line++;
		(*word_length)++;
	}
	word[i] = '\0';
	return word;
}


/*skips spaces. return a pointer to the line without the spaces*/
char * skipspace(char *line) {
	if (line == NULL) {
		return NULL;
	}
	while (isspace(*line) != 0 && *line != '\n' && *line != '\0') {
		line++;
	}
	return line;
}

/*checks if the first word in the line is a label
return the label if it is
return null if its not a label*/

int isLabel(char *line,int line_num) {
	/*char *word = (char*)malloc(MAX_LABEL);*/
	if (isdigit(*line)) {
		error = LABEL_STARTS_WITH_DIGIT;
		showError(line_num);
	}
	while (!isspace(*line) && *line != '\n' && *line != '\0') {
		if (*line == ':') {
			return 1;
		}
		line++;
	}
	return 0;
}
/*returns 1 if the line is empty or a comment*/
int ignore(char *line) {
	if (line == NULL) {
		return 1;
	}
	if (*line == ';') {
		return 1;
	}
	if (*line == '\0') {
		return 1;
	}
	while (isspace(*line)) {
		line++;
		if (*line == '\0' || *line == '\n') {
			return 1;
		}
	}
	return 0;
}






