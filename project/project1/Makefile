all: main.o quicksort.o radixsort.o sortMergeJoin.o
	gcc main.o quicksort.o radixsort.o sortMergeJoin.o -o smj.exe

smj.o: types.h
	gcc -c main.c

quicksort.o: quicksort.h types.h
	gcc -c quicksort.c

radixsort.o: radixsort.h quicksort.h types.h
	gcc -c radixsort.c

sortMergeJoin.o: sortMergeJoin.h radixsort.h types.h
	gcc -c sortMergeJoin.c

clean:
	rm -f main.o
	rm -f quicksort.o
	rm -f radixsort.o
	rm -f sortMergeJoin.o
	rm -f smj.exe
	rm -f results.txt
	rm -f testResults.txt
