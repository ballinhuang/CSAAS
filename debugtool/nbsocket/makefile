CC := g++ -std=c++11
CPPFLAGS := -Wall -O2
INCLUDE := ../../lib
LIB := -lpthread
SRCS := $(shell ls *.cpp)
MODEL := ../../lib/Message.o ../../lib/cc_socket.o ../../lib/s_socket.o
OBJS:=$(subst .cpp,.o,$(SRCS))
RM:=rm -f

all: nbsocket clean

nbsocket: $(OBJS) $(MODEL)
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