#include "asb.h"
#include <stdio.h>

void showError(int line_num) {
	switch (error) {
	case LONG_LINE:
		printf("line %d is to long, max line length is 80 chars\n", line_num);
		break;
	case LABEL_STARTS_WITH_DIGIT:
		printf("line %d starts with a digit\n", line_num);
		break;
	case ILEAGEL_FIRST_CHAR:
		printf("line %d can only start with a char or ' . '\n", line_num);
		break;
	case SAME_NAME_LABEL:
		printf("line %d two labels with the same name\n", line_num);
		break;
	case ILEAGEL_COMMA_IN_DIRECTIVE_LINE:
		printf("line %d ileagel comma in directive line. two commas in a row or statment starts with comma\n", line_num);
		break;
	case ILEAGEL_ASCIZ_COMMAND:
		printf("line %d ileagel asciz line after asciz only a string between " "\n", line_num);
		break;
	case ILEAGEL_LABEL:
		printf("line %d ileagel label. label starts with a alphabetic letter only\n", line_num);
		break;
	case ILEAGEL_REGISTER:
		printf("line %d register must start with a $ sign and be a value between 0 - 31\n", line_num);
		break;
	case NUMBER_OF_REGISTERS:
		printf("line %d this type of command needs 3 registers\n", line_num);
		break;
	case LABEL_NOT_FOUND:
		printf("line %d somthing went wrong. couldnt find label\n", line_num);
		break;
	case ASSAMBLER_ERROR:
		printf("line %d error in assambler\n", line_num);
		break;
	case BRANCH_WITH_EXTERNAL_LABEL:
		printf("line %d branch command with external label not allowd\n", line_num);
	default:
		break;
	}
}