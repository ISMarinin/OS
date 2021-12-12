CC = gcc

all: run

run: libstat.a libdynn.so static_main.o dynamic_main.o
	$(CC) -o stat-prog static_main.o -L. -lstat -lm
	$(CC) -o dyn-prog dynamic_main.o -L. -ldynn -ldl

libstat.a: static_lib.o
	ar rc libstat.a static_lib.o

static_lib.o: static_lib.c
	$(CC) -c static_lib.c

static_main.o: static_main.c
	$(CC) -c static_main.c

libdynn.so: dynamic_lib.o
	$(CC) -shared -o libdynn.so dynamic_lib.o

dynamic_lib.o: dynamic_lib.c
	$(CC) -c -fPIC dynamic_lib.c -lm

dynamic_main.o: dynamic_main.c
	$(CC) -c dynamic_main.c -lm

clean: 
	rm *.o *.a libdynn.so stat-prog dyn-prog