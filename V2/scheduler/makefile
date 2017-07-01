CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: scheduler clean

scheduler: Message.o main.o cc_socket.o s_socket.o scheduler_service.o ScheHandler.o ScheHandlerFactory.o 
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o