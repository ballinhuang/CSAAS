CC = g++
INCLUDE = ../lib
LIB = -lpthread

all: subjob clean

subjob: job.o main.o
	${CC} *.o ${LIB} -I ${INCLUDE} -o $@
%.o: %.cpp
	${CC} $^ ${LIB} -I ${INCLUDE} -c
clean:
	rm -f *.o