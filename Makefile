all: main.c vina.c vina.h lz.c lz.h
	gcc -Wall -g -Wextra main.c vina.c lz.c -o vinac

clean:
	-rm -f *~ *.o *.out Vinac *.vc *.lz