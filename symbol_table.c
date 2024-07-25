#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include "asb.h"

nlist *symbols_table = NULL;

nlist * createTable(char *symbol, char *atributes, int value) {
	nlist *list = (nlist*)malloc(sizeof(nlist));
	if (list == NULL) {
		printf("Somthing went wrong trying to allocate symbol table\n");
		return NULL;
	}
	strcpy(list->symbol, symbol);
	strcpy(list->atributes, atributes);
	list->value = value;
	list->next = NULL;
	return list;
}

nlist * addNewEntry(char *symbol, char *atributes, int value) {
	nlist *list = NULL;
	if (symbols_table == NULL) {
		if ((symbols_table = createTable(symbol, atributes, value)) == NULL) {
			return NULL;
		}
		else {
			return symbols_table;
		}
	}
	else {
		list = symbols_table;
		while (list->next != NULL) {
			if (strcmp(list->symbol, symbol) == 0) {
				error = SAME_NAME_LABEL;
				return symbols_table;
			}
			list = list->next;
		}
		list->next = (nlist*)malloc(sizeof(nlist));
		list = list->next;
		strcpy(list->symbol, symbol);
		strcpy(list->atributes, atributes);
		list->value = value;
		list->next = NULL;
		list = symbols_table;
		return list;
	}
}

void freeSymbolTable() {
	nlist *temp;
	while (symbols_table != NULL) {
		temp = symbols_table;
		symbols_table = symbols_table->next;
		free(temp);
	}
}

void printSymboleTable() {
	nlist *list = symbols_table;
	while (list != NULL) {
		printf("symbol: %s atribuits: %s value: %d\n", list->symbol, list->atributes,list->value);
		list = list->next;
	}
}

nlist * findLabel(char *label) {
	nlist *list;
	list = symbols_table;
	while (list != NULL) {
		if (strcmp(list->symbol, label) == 0) {
			return list;
		}
		list = list->next;
	}

	return NULL;
}


void updateAdressSymbolTabel() {
	nlist *list;
	int val;
	list = NULL;
	list = symbols_table;
	while (symbols_table != NULL) {
		if (strcmp(symbols_table->atributes, "data") == 0) {
			val = symbols_table->value + icf;
			symbols_table->value = val;
		}
		symbols_table = symbols_table->next;
	}
	symbols_table = list;
}

