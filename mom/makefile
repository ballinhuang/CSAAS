CC := g++ -std=c++11
CPPFLAGS := -Wall -O2
INCLUDE := ../lib
LIB := -pthread
SRCS := $(shell ls *.cpp)
MODEL := ../lib/s_socket.o ../lib/cc_socket.o ../lib/Message.o ../lib/s_nbsocket.o
OBJS:=$(subst .cpp,.o,$(SRCS))
RM:=rm -f

all:
	$(MAKE) mom
	$(MAKE) clean

mom: $(OBJS) $(MODEL)
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