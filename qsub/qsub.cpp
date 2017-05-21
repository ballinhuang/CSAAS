#include <iostream>
#include "qsub_function.h"
#include "qsub_client_socket.h"
using namespace std;

int main(int argc, char **argv, char **envp)
{
	string job_data = get_job_info(argc,argv,envp);

	if(job_data == "") {
		cout << "job_data can't get the correct string !" << endl;
		return 0;
	}

	qsub(job_data);

	return 0;
}