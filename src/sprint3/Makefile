main: main.o functions.o thread_functions.o us364.o
	gcc main.o functions.o thread_functions.o us364.o -z noexecstack -o main -lrt -lpthread

main.o: main.c functions.h structs.h thread_functions.h us364.h
	gcc -g -Wall -Wextra -fanalyzer -c main.c -o main.o -lrt -lpthread
	
us364.o: us364.c us364.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us364.c -o us364.o -lrt -lpthread

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
