#include <stdio.h>
#include "asb.h"
#include <limits.h>

/*sends one file at a time to assemble function to be proccesed*/


int main(int argc, char *argv[]) {
	int i;
	for (i = 1; i < argc; i++) {
		if (assemble(argv[i]) ==1){
		createOutPutFiles(argv[i]);
		freeDataImage();
		freeSymbolTable();
		freeCommandExt();
	}
   }
	return 0;
}
