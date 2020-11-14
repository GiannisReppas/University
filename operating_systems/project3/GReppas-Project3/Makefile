all: myport monitor port-master vessel

myport: itoa_plus_nod.o myport.o
	gcc itoa_plus_nod.o myport.o -o myport -lpthread

myport.o: myport.c itoa_plus_nod.h types.h
	gcc -c myport.c

itoa_plus_nod.o: itoa_plus_nod.c itoa_plus_nod.h
	gcc -c itoa_plus_nod.c

monitor: itoa_plus_nod.o monitor.o
	gcc itoa_plus_nod.o monitor.o -o monitor -lpthread

monitor.o: monitor.c itoa_plus_nod.h types.h
	gcc -c monitor.c

port-master: port-master.o
	gcc port-master.o -o port-master -lpthread

port-master.o: types.h
	gcc -c port-master.c

vessel: vessel.o
	gcc vessel.o -o vessel -lpthread

vessel.o: types.h
	gcc -c vessel.c

clean:
	rm -f itoa_plus_nod.o
	rm -f myport.o
	rm -f myport
	rm -f monitor.o
	rm -f monitor
	rm -f port-master.o
	rm -f port-master
	rm -f vessel.o
	rm -f vessel
	rm -f results.txt
