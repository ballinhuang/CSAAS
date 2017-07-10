CC := g++ -std=c++11
CPPFLAGS := -Wall -O2
INCLUDE := ../lib
LIB := -lpthread
SRCS := $(shell ls *.cpp)
MODEL := ../lib/s_socket.o ../lib/c_socket.o ../lib/Message.o
OBJS:=$(subst .cpp,.o,$(SRCS))
RM:=rm -f

all: server clean

server: $(OBJS) $(MODEL)
	@echo "------ Linking ------" && \
	$(CC) $(CPPFLAGS) $(LIB) -I ${INCLUDE} $^ -o $@ && \
	echo  "------ Success ------" || echo "------ Fail! ------"

%.o: %.cpp
	@echo "------ Compile >> " $^ "to" $@ && \
	$(CC) $(CPPFLAGS) ${LIB} -I ${INCLUDE} -c $^ -o $@ && \
	echo  "------ Success ------" || echo "------ Fail! ------"

clean:
	@$(RM) $(OBJS) && \
	echo "------ Clear ------"