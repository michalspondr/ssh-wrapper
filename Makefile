CC=gcc
CFLAGS=-O2


ssh-wrapper: ssh-wrapper.c ssh-wrapper.h includes.h fifo.o fork.o malloc.o pts.o str.o
	$(CC) $(CFLAGS) -o ssh-wrapper ssh-wrapper.c fifo.o fork.o malloc.o pts.o str.o
	cp ssh-wrapper ../lib/ &&chmod o+x ../lib/ssh-wrapper
	
fifo.o: fifo.c fifo.h includes.h
	./defines_create.sh	#vytvorim prislusny hlavickovy soubor
	$(CC) $(CFLAGS) -c fifo.c
fork.o: fork.c fork.h includes.h
	$(CC) $(CFLAGS) -c fork.c
malloc.o: malloc.c malloc.h includes.h
	$(CC) $(CFLAGS) -c malloc.c
pts.o: pts.c pts.h includes.h
	$(CC) $(CFLAGS) -c pts.c
str.o: str.c str.h includes.h
	$(CC) $(CFLAGS) -c str.c

clean:
	rm -f *.o ssh-wrapper defines.h

