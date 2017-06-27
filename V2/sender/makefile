CC = g++
INCLUDE = ../lib

all: sender clean

sender: c_socket.o Message.o main.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o