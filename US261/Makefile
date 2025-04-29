us261: us261.o functions.o
	gcc us261.o functions.o -z noexecstack -o main

us261.o: us261.c functions.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us261.c -o us261.o

functions.o: functions.c functions.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c functions.c -o functions.o

run: us261
	./main

clean:
	rm -f *.o main

vg: us261
	valgrind ./main

debug: us261
	gdb -tui main

ddd: us261
	ddd main &