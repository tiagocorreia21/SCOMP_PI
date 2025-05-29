main: main.o functions.o thread_functions.o
	gcc main.o functions.o thread_functions.o -z noexecstack -o main -lrt -lpthread

main.o: main.c functions.h structs.h thread_functions.h
	gcc -g -Wall -Wextra -fanalyzer -c main.c -o main.o -lrt -lpthread

run: main
	./main

clean:
	rm -f *.o main

vg: main
	valgrind ./main

debug: main
	gdb -tui main

ddd: main
	ddd main &
