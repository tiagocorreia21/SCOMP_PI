us261: us261.o functions.o us263.o us264.o us265.o
	gcc us261.o functions.o us263.o us264.o us265.o -z noexecstack -o main -lrt -lpthread

us261.o: us261.c functions.h structs.h us264.h
	gcc -g -Wall -Wextra -fanalyzer -c us261.c -o us261.o -lrt -lpthread

us263.o: us263.c us263.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us263.c -o us263.o -lrt -lpthread

us264.o: us264.c us264.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us264.c -o us264.o -lrt -lpthread
	
us265.o: us265.c us265.h structs.h
	gcc -g -Wall -Wextra -fanalyzer -c us265.c -o us265.o -lrt -lpthread

functions.o: functions.c functions.h structs.h us263.h
	gcc -g -Wall -Wextra -fanalyzer -c functions.c -o functions.o -lrt -lpthread

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
