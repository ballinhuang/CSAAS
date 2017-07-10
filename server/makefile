CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: server clean

server: Server.o Monitor.o main.o ../lib/s_socket.o HandlerFactory.o ../lib/c_socket.o ../lib/Message.o Handler.o Node.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o