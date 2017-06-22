CC = g++
LIB = -lpthread

all: server clean

server: server_socket_daemon.o job_info.o
	${CC} *.o ${LIB} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -c


clean:
	rm -f *.o