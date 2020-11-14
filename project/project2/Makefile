all: main.o sortMergeJoin.o radixsort.o quicksort.o read_files.o read_queries.o execute_query.o sums.o updates.o
	gcc main.o sortMergeJoin.o radixsort.o quicksort.o read_files.o read_queries.o execute_query.o sums.o updates.o -o main.exe

main.o: types.h radixsort.h quicksort.h sortMergeJoin.h read_files.h read_queries.h
	gcc -c main.c

quicksort.o: quicksort.h types.h
	gcc -c quicksort.c

radixsort.o: radixsort.h quicksort.h types.h
	gcc -c radixsort.c

sortMergeJoin.o: sortMergeJoin.h radixsort.h types.h
	gcc -c sortMergeJoin.c

read_files.o: read_files.h types.h
	gcc -c read_files.c

read_queries.o: read_queries.h types.h
	gcc -c read_queries.c

execute_query.o: execute_query.h updates.h sums.h sortMergeJoin.h types.h
	gcc -c execute_query.c

sums.o: sums.h  types.h
	gcc -c sums.c

updates.o: updates.h types.h
	gcc -c updates.c

clean:
	rm -f updates.o
	rm -f sums.o
	rm -f execute_query.o
	rm -f read_queries.o
	rm -f read_files.o
	rm -f main.o
	rm -f sortMergeJoin.o
	rm -f radixsort.o
	rm -f quicksort.o
	rm -f main.exe
	rm -f results.txt
