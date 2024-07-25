#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include "asb.h"
#include <limits.h>
#include <stdlib.h>

commandExternal *command_external = NULL;
int line_num = 1;

int secondPass(char *file_name) {
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
			
			
			if (readLineSecondPass(line) == 0) {
				printf("assembly stoped due to an error\n");
				return 0;
				break;
			}
		}
		line_num++;
		
	}

	return 1;
}

int readLineSecondPass(char* line) {
	int curr_word_leng = 0;
	char *current_word;
	nlist *item;
	line = skipspace(line);
	current_word = getWord(line, &curr_word_leng);
	line = moveToNextWord(line, curr_word_leng);

	if (isLabel(current_word, line_num) == 1) {
		free(current_word);
		current_word = getWord(line, &curr_word_leng);
		line = moveToNextWord(line, curr_word_leng);
	}

	if (isDirective(current_word) != 0) {
		if (strcmp(current_word, ".entry") == 0) {
			char label[MAX_LABEL];
			int i = 0;
			while (*line != '\n'&& *line != '\0' && isspace(*line) == 0) {
				label[i++] = *line;
				line++;
			}
			label[i] = '\0';
			if ((item = findLabel(label)) == NULL) {
				error = LABEL_NOT_FOUND;
				return 0;
				showError(line_num);
			}
			strcat(item->atributes, ", entry");
			return 1;
		}
		else {
			return 1;
		}
	}

	if (handleActionSecondPass(line, current_word) == 0) {
		return 0;
	}
	ic += 4;
	
	
	free(current_word);
	return 1;

}

int handleActionSecondPass(char *line, char *action) {
	int i;
	for (i = 0; i < ACTIONS_NUM; i++) {
		if (strcmp(actions_struct[i].word, action) == 0) {
			return processActionSecondPass(line, actions_struct[i]);
		}
	}
	return 0;
}

/*identifys what type is the actions and send it to the propper function to continue processing*/
int processActionSecondPass(char *line, actions command) {
	switch (command.type) {
	case 'r':
		break;
	case 'i':
		return processItypeSecondPass(line, command);
		break;
	case 'j':
		return processJtypeSecondPass(line, command);
		break;
	}
	return 1;
}


int processItypeSecondPass(char *line, actions command) {
	char label[MAX_LABEL];
	char *temp;
	int comma_counter;
	int i;
	int dist;
	nlist *item;
	DataImage *image;
	comma_counter = 0;
	i = 0;
	if (command.op_code >= 15 && command.op_code <= 18) {
		while (*line != '\0' && *line != '\n') {
			if (*line == ',' && comma_counter < 2) {
				comma_counter++;
			}
			if (comma_counter == 2) {
				line = skipComma(line);
				line = skipspace(line);
				label[i++] = *line;
			}
			line++;
		}
		label[i] = '\0';
		item = findLabel(label);
		if (item == NULL) {
			error = LABEL_NOT_FOUND;
			showError(line_num);
			return 0;
		}
		if (strcmp(item->atributes, "extern") == 0) {
			error = BRANCH_WITH_EXTERNAL_LABEL;
			showError(line_num);
		}
		image = findLineByIc(ic);
		dist = item->value - ic;
		temp = int2bin(dist, 16);
		strcat(image->machine_code, temp);
		free(temp);
	}
	
	return 1;
}

char * skipComma(char *line) {
	if (*line == ',') {
		line++;
	}
	return line;
}


int processJtypeSecondPass(char *line, actions command) {
	char label[MAX_LABEL];
	nlist *symbol;
	char *code;
	int i = 0;
	DataImage *item;
	commandExternal *list;
	if (command.op_code == 63) {
		return 1;
	}

	if (command.op_code == 30) {
		if (*line == '$') {
			return 1;
		}
	}

	while (*line != '\0' && *line != '\n' && isspace(*line) == 0) {
		label[i++] = *line;
		line++;
	}
	label[i] = '\0';

	
	item = findLineByIc(ic);
	if (item == NULL) {
		error = ASSAMBLER_ERROR;
		showError(line_num);
		return 0;
	}

	symbol = findLabel(label);

	if (symbol == NULL) {
		error = LABEL_NOT_FOUND;
		showError(line_num);
		return 0;
	}
	code = int2bin(symbol->value, 25);
	strcat(item->machine_code, code);
	free(code);
	
	if (strcmp(symbol->atributes, "extern") == 0) {
		if (command_external == NULL) {
			command_external = (commandExternal*)malloc(sizeof(commandExternal));
			command_external->address = item->address;
			strcpy(command_external->name, label);
			command_external->next = NULL;
		}
		else {
			list = command_external;
			while (list->next != NULL) {
				list = list->next;
			}
			list->next = (commandExternal*)malloc(sizeof(commandExternal));
			list = list->next;
			list->address = item->address;
			strcpy(list->name, label);
			list->next = NULL;
		}
	}

	return 1;
}


void freeCommandExt() {
	commandExternal *temp;
	while (command_external != NULL) {
		temp = command_external;
		command_external = command_external->next;
		free(temp);
	}
}