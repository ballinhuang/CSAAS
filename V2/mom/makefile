CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: mom clean

mom: Mom.o main.o s_socket.o MomHandlerFactory.o c_socket.o Message.o MomHandler.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o