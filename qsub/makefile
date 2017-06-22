CC = g++
LIB = -lpthread

all: subjob clean

subjob: job_info.o qsub_client_socket.o qsub.o qsub_function.o
	${CC} *.o ${LIB} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -c

clean:
	rm -f *.o