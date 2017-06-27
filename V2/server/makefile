CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: server clean

server: Server.o Monitor.o main.o s_socket.o HandlerFactory.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o