#include <iostream>
#include <vector>
#include <chrono>
#include <cstdio>
#include "ThreadPool.h"
#include <mutex>


std::mutex mtx;

class MySingleton
{
public:
      static MySingleton * GetInstance()
      //static method that returns only instance of MySingletone
      {
           if (m_pOnlyOneInstance == NULL)
			{
					mtx.lock();
					if (m_pOnlyOneInstance == NULL)
					// Solution 1 and 2 gaps addressed by moving
					// critical section block and by re-doing this check!
					{
						m_pOnlyOneInstance = new MySingleton();
						printf("creat instance\n");
					}
					mtx.unlock();
			}
			return m_pOnlyOneInstance;
      }

private:

      static MySingleton * m_pOnlyOneInstance;
      //holds one and only object of MySingleton
	  std::vector<int> a;
      MySingleton(){
		  a = std::vector<int>();
	  };// private constructor
	  std::mutex atx;
public:
      void foo(int s){
		  atx.lock();
		  a.push_back(1);
		  atx.unlock();
		  printf("job %d call foo.\n",s);
	  }
	  void zoo(int s){
		  atx.lock();
		  printf("job %d call zoo : value a= %d\n",s,a.size());
		  atx.unlock();
	  }

};

MySingleton *MySingleton::m_pOnlyOneInstance=0;
void f(int s){
	MySingleton::GetInstance()->foo(s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down\n",s);
}
void x(int s){
	MySingleton::GetInstance()->zoo(s);
	std::this_thread::sleep_for(std::chrono::seconds(1));
	printf("job %d down\n",s);
}
int main()
{

	ThreadPool pool(25);
	std::vector< std::future<int> > results;

	for(int i = 1; i < 500; ++i) {
		int arg = i;
		if(i % 5 != 0)
		pool.enqueue(&f,arg);
		else
		pool.enqueue(&x,arg);

	}
	return 0;
}
