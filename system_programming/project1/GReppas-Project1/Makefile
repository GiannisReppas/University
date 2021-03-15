bitcoin: main.o take_info_from_command_line.o initialize_hash_tables.o print_hash_tables.o hash_function.o functions_for_error_checking.o transactionsFile_functions.o bitCoinBalancesFile_functions.o itoa_plus_nod.o free_memory.o menu.o
	gcc main.o take_info_from_command_line.o menu.o initialize_hash_tables.o print_hash_tables.o hash_function.o functions_for_error_checking.o transactionsFile_functions.o bitCoinBalancesFile_functions.o itoa_plus_nod.o free_memory.o -o bitcoin

main.o: types.h functions_for_error_checking.h hash_function.h bitCoinBalancesFile_functions.h transactionsFile_functions.h free_memory.h itoa_plus_nod.h print_hash_tables.h initialize_hash_tables.h take_info_from_command_line.h menu.h
	gcc -c main.c

free_memory.o: types.h free_memory.h
	gcc -c free_memory.c

bitCoinBalancesFile_functions.o: types.h functions_for_error_checking.h hash_function.h bitCoinBalancesFile_functions.h
	gcc -c bitCoinBalancesFile_functions.c

functions_for_error_checking.o: types.h hash_function.h functions_for_error_checking.h
	gcc -c functions_for_error_checking.c

hash_function.o: hash_function.h
	gcc -c hash_function.c

initialize_hash_tables.o: types.h initialize_hash_tables.h
	gcc -c initialize_hash_tables.c

itoa_plus_nod.o: itoa_plus_nod.h
	gcc -c itoa_plus_nod.c

menu.o: types.h hash_function.h functions_for_error_checking.h transactionsFile_functions.h itoa_plus_nod.h menu.h
	gcc -c menu.c

print_hash_tables.o: types.h print_hash_tables.h
	gcc -c print_hash_tables.c

take_info_from_command_line.o: take_info_from_command_line.h
	gcc -c take_info_from_command_line.c

transactionsFile_functions.o: types.h hash_function.h transactionsFile_functions.h
	gcc -c transactionsFile_functions.c

clean:
	rm -f main.o
	rm -f bitcoin
	rm -f free_memory.o
	rm -f itoa_plus_nod.o
	rm -f bitCoinBalancesFile_functions.o
	rm -f transactionsFile_functions.o
	rm -f functions_for_error_checking.o
	rm -f hash_function.o
	rm -f print_hash_tables.o
	rm -f initialize_hash_tables.o
	rm -f menu.o
	rm -f take_info_from_command_line.o
