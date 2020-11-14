all: mirror_client reader writer random_dir_name random_text

writer: writer.o itoa_plus_nod.o
	gcc itoa_plus_nod.o writer.o -o writer

writer.o: itoa_plus_nod.h
	gcc -c writer.c

reader: reader.o itoa_plus_nod.o
	gcc itoa_plus_nod.o reader.o -o reader

reader.o: itoa_plus_nod.h
	gcc -c reader.c

mirror_client: mirror_client.o itoa_plus_nod.o
	gcc mirror_client.o itoa_plus_nod.o -o mirror_client

mirror_client.o: itoa_plus_nod.h
	gcc -c mirror_client.c

itoa_plus_nod.o: itoa_plus_nod.h
	gcc -c itoa_plus_nod.c

random_dir_name:
	gcc random_dir_name.c -o random_dir_name

random_text:
	gcc random_text.c -o random_text

clean:
	rm -f itoa_plus_nod.o
	rm -f mirror_client.o
	rm -f mirror_client
	rm -f writer.o
	rm -f reader.o
	rm -f writer
	rm -f reader
	rm -f random_dir_name
	rm -f random_text
