CC := g++ -std=c++11
CPPFLAGS := -Wall -O2
INCLUDE := ../lib
LIB := -lpthread
SRCS := $(shell ls *.cpp)
OBJS:=$(subst .cpp,.o,$(SRCS))
RM:=rm -f

.PHONY: lib

all: lib

lib: $(OBJS)

%.o: %.cpp
	@echo "------ Compile >> " $^ "to" $@ && \
	$(CC) $(CPPFLAGS) ${LIB} -I ${INCLUDE} -c $^ -o $@ && \
	echo  "------ Success ------" || echo "------ Fail! ------"

clean:
	@$(RM) $(OBJS) && \
	echo "------ Clear ------"