CC = g++
LIB = -lpthread

all: scheduler clean

scheduler: scheduler_socket_daemon.o
	${CC} *.o ${LIB} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -c


clean:
	rm -f *.o