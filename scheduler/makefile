CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: scheduler clean

scheduler: ../lib/Message.o main.o ../lib/cc_socket.o ../lib/s_socket.o scheduler_service.o ScheHandler.o ScheHandlerFactory.o 
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o