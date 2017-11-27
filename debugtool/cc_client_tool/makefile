CC := g++ -std=c++11
CPPFLAGS := -Wall -O2
INCLUDE := ../../lib
LIB := -pthread
SRCS := $(shell ls *.cpp)
MODEL := ../../lib/cc_socket.o
OBJS:=$(subst .cpp,.o,$(SRCS))
RM:=rm -f

all: cc_client clean

cc_client: $(OBJS) $(MODEL)
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