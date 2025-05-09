us261: us261.o functions.o us263.o
	gcc us261.o functions.o us263.o -z noexecstack -o main

us261.o: us261.c functions.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us261.c -o us261.o

functions.o: functions.c functions.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c functions.c -o functions.o

us263.o: us263.c structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us263.c -o us263.o

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