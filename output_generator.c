#include "asb.h"
#include <math.h>
#include <stdio.h>

int externals = 0;
int entris = 0;

void createOutPutFiles(char *file_name) {
	char *output_file;
	char *output_file_ob;
	char *output_file_ent;
	char *output_file_ext;
	output_file = removeExtension(file_name);
	output_file_ext = addExtension(output_file, 2);
	output_file_ent = addExtension(output_file, 1);
	output_file_ob = addExtension(output_file, 0);
	
	createObOutput(output_file_ob);
	
	createEntOutput(output_file_ent);
	
	createExtOutput(output_file_ext);

	free(output_file);
	free(output_file_ob);
	free(output_file_ent);
	free(output_file_ext);
}

char * addExtension(char *name, int extension) {
	char *file = (char*)malloc(30);
	strcpy(file, name);
	switch (extension) {
	case 0:
		strcat(file, ".ob");
		break;
	case 1:
		strcat(file, ".ent");
		break;
	case 2:
		strcat(file, ".ext");
		break;
	 }
	return file;
}
char *removeExtension(char *name) {
	char *file = (char*)malloc(30);
	int i = 0;

	while (*name != '.') {
		file[i++] = *name;
		name++;
	}

	file[i] = '\0';
	return file;
}

void createExtOutput(char *file_name) {
	FILE *file = NULL;
	commandExternal *list = command_external;
	char *address;
	while (list != NULL) {
		if (file == NULL) {
			file = fopen(file_name, "w");
		}
		fprintf(file, "%s ", list->name);
		address = intToString(list->address);
		fprintf(file, "%s\n", address);
		free(address);
		list = list->next;
	}
}

void createEntOutput(char *file_name) {
	int i = 0;
	int j = 0;
	char temp[8];
	char *address;
	nlist *list = symbols_table;
	FILE *file;
	file = NULL;
	while (list != NULL) {
		while (list->atributes[i] != '\0' && list->atributes[i] != '\n') {
			if (list->atributes[i] == ',') {
				i++;
				while (isspace(list->atributes[i]) != 0) {
					i++;
				}
				while (list->atributes[i] != '\0' && list->atributes[i] != '\n' && isspace(list->atributes[i]) == 0) {
					temp[j++] = list->atributes[i++];
				}
				temp[j] = '\0';
				if (strcmp(temp, "entry") == 0) {
					if (file == NULL) {
						file = fopen(file_name, "w");
					}
					fprintf(file, "%s ", list->symbol);
					address = intToString(list->value);
					fprintf(file, "%s\n", address);
					free(address);
					j = 0;
				}
			}
			i++;
		}
		list = list->next;
		i = 0;
	}
}

void createObOutput(char *file_name) {
	int divert;
	int i = 0;
	int j = 0;
	int k = 0;
	char *str;
	char temp[9];
	int argument_counter = 0;
	int value = 100;
	int printing_data = 0;
	DataImage *list;
	FILE *file;
	list = NULL;
	file = fopen(file_name, "w");
	if (file == NULL) {
		printf("unable to create output file\n");
	}
	else {
		fprintf(file, "%7d %d", (icf - 100), dcf);
		
		list = data_image;
		divert = strlen(list->machine_code);
		for (k = 0; k < 2; k++) {
			while (list != NULL) {
				if ((list->is_data == 0 && printing_data == 0 )|| (list->is_data == 1 && printing_data == 1)) {
					if (argument_counter == 0) {
						str = intToString(value);
						value += 4;
						fprintf(file, "\n%s", str);
						free(str);
					}
					for (i = 8; i > 0; i--) {
						temp[j++] = list->machine_code[divert - i];
					}
					divert -= 8;
					temp[j] = '\0';
					j = 0;
					str = binaryByteToHexa(temp);
					fprintf(file, " %s", str);
					free(str);
					argument_counter = (argument_counter + 1) % 4;
					if (divert <= 0) {
						list = list->next;
						if (list != NULL) {
							divert = strlen(list->machine_code);
						}
					}
				}
				else {
					list = list->next;
					if (list != NULL) {
						divert = strlen(list->machine_code);
					}
				}
			}
			list = data_image;
			divert = strlen(list->machine_code);
			printing_data = 1;
		}
	}
}

char * intToString(int value) {
	char a, b;
	int i;
	char *text = (char*)malloc(5);
	sprintf(text, "%d",value);
	if (text[3] == '\0') {
		a = text[0];
		text[0] = '0';
		for (i = 1; i < 4; i++) {
			b = text[i];
			text[i] = a;
			a = b;
		}
		text[i] = '\0';
	}
	return text;
}

char *binaryByteToHexa(char *binary) {
	char hex_arr[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	char *hex;
	int number;
	int power;
	int i;
	int j;
	j = 0;
	number = 0;
	i = 0;
	power = 3;
	hex = (char*)malloc(3);
	for (j =0; j < 2; j++) {
		for (i = 0; i < 4; i++) {
			number += (twoPower(power)) * ((*binary) - '0');
			power--;
			binary++;
		}
		hex[j] = hex_arr[number];
		number = 0;
		power = 3;
	}
	hex[j] = '\0';

	return hex;
}

int twoPower(int i) {
	int sum;
	int j;
	sum = 1;
	for (j = 0; j < i; j++) {
		sum = sum * 2;
	}
	return sum;
}