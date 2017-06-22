CC = g++
LIB = -lpthread

all: mom clean

mom: mom_daemon.o
	${CC} *.o ${LIB} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -c


clean:
	rm -f *.o