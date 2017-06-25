#include <iostream>
#include <vector>
#include <chrono>
#include <cstdio>
#include "ThreadPool.h"

void f(int s){
    printf("job %d creat!\n",s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down!\n",s);
}

int main()
{

	ThreadPool pool(3);
	for(int i = 0; i < 15; ++i) {
		int arg = i;
		pool.enqueue(&f,arg);
	}
	return 0;
}
