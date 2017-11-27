#include "cc_socket.hpp"
#include <iostream>
#include <ThreadPool.h>

using namespace std;

int main(int argc, char **argv)
{
    string ip, port;
    if (argc > 1)
    {
        ip = string(argv[1]);
        port = string(argv[2]);
    }
    else
    {
        cerr << "arg:[ip] [port]\n";
        exit(1);
    }
    ThreadPool pool(10);
    for (int i = 0; i < 10; i++)
    {
        pool.enqueue([i, ip, port] {
            cc_socket cs;
            string back;
            int rc;
            rc = cs.setConnection(ip, port);

            cs.connect2server();
            cs.send("Hello1");
            back = cs.receive();
            if (back != "Hello1")
            {
                cerr << i << " Error at Hello1" << endl;
            }
            //cout << back << endl;
            cs.send("Hello2");
            back = cs.receive();
            //cout << back << endl;
            if (back != "Hello2")
            {
                cerr << i << " Error at Hello2" << endl;
            }
            cs.send("Hello3");
            //back = cs.receive();
            //cout << back << endl;
            /*
            if (back != "Hello3")
            {
                cerr << i << " Error at Hello3" << endl;
            }
            */
            cs.closeConnection();
            while (1)
                ;
        });
    }
}