#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include "asb.h"

DataImage *data_image = NULL;


DataImage *createImageTable(int address, char *machine_code, int is_data, int dc,int ic) {
	DataImage *head = NULL;
	head = (DataImage*)malloc(sizeof(DataImage));
	if (head == NULL) {
		printf("Somthing went wrong trying to allocate data image table\n");
		return NULL;
	}
	head->address = address;
	strcpy(head->machine_code, machine_code);
	head->is_data = is_data;
	head->dc = dc;
	head->ic = ic;
	head->next = NULL;
	return head;
}

DataImage * addNewImageEntry(int address, char *machine_code, int is_data, int dc, int ic) {
	DataImage *list = NULL;
	if (data_image == NULL) {
		if ((data_image = createImageTable(address,machine_code,is_data,dc,ic)) == NULL) {
			return NULL;
		}
		else {
			return data_image;
		}
	}
	else {
		list = data_image;
		while (list->next != NULL) {
			list = list->next;
		}
		list->next = (DataImage*)malloc(sizeof(DataImage));
		list = list->next;
		list->address = address;
		strcpy(list->machine_code, machine_code);
		list->is_data = is_data;
		list->dc = dc;
		list->ic = ic;
		list->next = NULL;
		list = data_image;
		return list;
	}

}


void printDataImage() {
	DataImage *list = data_image;
	printf("pritnig data image: \n\n");
	while (list != NULL) {
		printf("address: %d  is data: %d  dc: %d  ic: %d  machine code: %s",list->address,list->is_data,list->dc,list->ic,
			list->machine_code);	
		printf("\n\n");
		list = list->next;
	}
}

void freeDataImage() {
	DataImage *temp;
	while (data_image != NULL) {
		temp = data_image;
		data_image = data_image->next;
		free(temp);
	}
}

void add_machine_code(DataImage *data, char *code) {
	strcat(data->machine_code, code);
}

/*converts int to binary string size represents the num of cherecters*/
char * int2bin(int i, int size)
{
	int k;
	int j = 0;
	int position = 32 - size;
	int u = i;
	char *code = (char*)malloc(size + 1);
	size_t bits = sizeof(int) * CHAR_BIT;
	char * str = (char*)malloc(bits + 1);
	if (!str) return NULL;
	str[bits] = '\0';

	
	/*(unsigned *)&i;*/
	for (; bits--; u >>= 1)
		str[bits] = u & 1 ? '1' : '0';

	for (k = position; k < 33; k++) {
		code[j++] = str[k];
	}
	free(str);
	return code;
}



DataImage * findLineByIc(int ic) {
	DataImage *list;
	list = data_image;
	while (list != NULL) {
		if (list->address == ic) {
			return list;
		}
		list = list->next;
	}
	return NULL;
}


void updateAdressDataImage() {
	DataImage *list;
	list = data_image;
	while (list != NULL) {
		if (list->is_data == 1) {
			list->address = list->dc + icf;
		}
		list = list->next;
	}
}