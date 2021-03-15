all: dropbox_server temp_client

dropbox_server: dropbox_server.o itoa_plus_nod.o
	gcc dropbox_server.o itoa_plus_nod.o -o dropbox_server -lpthread

dropbox_server.o: itoa_plus_nod.h
	gcc -c dropbox_server.c

temp_client: temp_client.o itoa_plus_nod.o
	gcc temp_client.o itoa_plus_nod.o -o temp_client

temp_client.o: itoa_plus_nod.h
	gcc -c temp_client.c

itoa_plus_nod.o: itoa_plus_nod.h
	gcc -c itoa_plus_nod.c

clean:
	rm -f dropbox_server.o
	rm -f dropbox_server
	rm -f temp_client.o
	rm -f temp_client
	rm -f itoa_plus_nod.o
