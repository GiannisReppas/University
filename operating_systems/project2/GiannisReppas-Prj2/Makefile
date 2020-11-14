all: leaf_node spliter_merger myfind

leaf_node: itoa_plus_nod.o leaf_node.o
	gcc itoa_plus_nod.o leaf_node.o -o leaf_node -lm

leaf_node.o: leaf_node.c itoa_plus_nod.h types.h
	gcc -c leaf_node.c

itoa_plus_nod.o: itoa_plus_nod.c itoa_plus_nod.h
	gcc -c itoa_plus_nod.c

spliter_merger: itoa_plus_nod.o spliter_merger.o
	gcc itoa_plus_nod.o spliter_merger.o -o spliter_merger

spliter_merger.o: spliter_merger.c itoa_plus_nod.h types.h
	gcc -c spliter_merger.c

myfind: itoa_plus_nod.o myfind.o
	gcc itoa_plus_nod.o myfind.o -o myfind

myfind.o: myfind.c itoa_plus_nod.h types.h
	gcc -c myfind.c

clean:
	rm -f leaf_node.o
	rm -f itoa_plus_nod.o
	rm -f leaf_node
	rm -f spliter_merger.o
	rm -f spliter_merger
	rm -f myfind.o
	rm -f myfind
	rm -f results.txt
