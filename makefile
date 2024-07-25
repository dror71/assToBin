asmb: data_image.o errors.o output_generator.o first_pass.o symbol_table.o main.o second_pass.o
	gcc -ansi -pedantic -g -Wall data_image.o errors.o output_generator.o first_pass.o symbol_table.o main.o second_pass.o -o asmb
	
data_image.o: data_image.c asb.h 
	gcc -c -ansi -pedantic -g -Wall data_image.c -o data_image.o 
errors.o: errors.c asb.h 
	gcc -c -ansi -pedantic -g -Wall errors.c -o errors.o 
output_generator.o: output_generator.c asb.h 
	gcc -c -ansi -pedantic -g -Wall output_generator.c -o output_generator.o 
first_pass.o: first_pass.c asb.h 
	gcc -c -ansi -pedantic -g -Wall first_pass.c -o first_pass.o 
symbol_table.o: symbol_table.c asb.h 
	gcc -c -ansi -pedantic -g -Wall symbol_table.c -o symbol_table.o 
main.o: main.c asb.h 
	gcc -c -ansi -pedantic -g -Wall main.c -o main.o 
second_pass.o: second_pass.c asb.h 
	gcc -c -ansi -pedantic -g -Wall second_pass.c -o second_pass.o 
