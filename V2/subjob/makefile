CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: subjob clean

subjob: ../lib/Message.o main.o ../lib/cc_socket.o subjob_service.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o