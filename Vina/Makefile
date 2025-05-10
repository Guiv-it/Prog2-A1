all: main.c vina.c vina.h lista.c lista.h lz.c lz.h
	gcc -Wall main.c vina.c lista.c lz.c -o Vinac

Valgrind: main.c vina.c vina.h lista.c lista.h lz.c lz.h
	valgring -Wall main.c vina.c lista.c lz.c -o Vinac

clean:
	-rm -f *~ *.o *.out