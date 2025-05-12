all: main.c vina.c vina.h lz.c lz.h
	gcc -Wall -g -Wextra main.c vina.c lz.c -o Vinac

Valgrind: main.c vina.c vina.h lz.c lz.h
	valgring -Wall main.c vina.c lz.c -o Vinac

clean:
	-rm -f *~ *.o *.out Vinac *.vc