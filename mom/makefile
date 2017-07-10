CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: mom clean

mom: Mom.o main.o ../lib/s_socket.o MomHandlerFactory.o ../lib/c_socket.o ../lib/Message.o MomHandler.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o