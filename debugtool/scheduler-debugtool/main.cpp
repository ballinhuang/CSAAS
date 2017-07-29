
#include "Message.hpp"
#include "s_socket.hpp"
#include "cc_socket.hpp"
#include "json.hpp"
#include <iostream>
#include <string>
#include <map>
using namespace std;
// for convenience
using json = nlohmann::json;

int main()
{

    int lab;
    int cpu;
    map<string, int> cluster;

    cout << "Enter the node count. < Example : 10 > :" << endl;
    cin >> lab;
    if (lab < 0)
    {
        cout << "Wrong node count " << lab << "." << endl;
        exit(1);
    }
    for (int i = 0; i < lab; i++)
    {
        string labname = "lab" + to_string(i);

        cout << "Enter the "
             << "lab" << i << "'s  CPU core. < Example : 3 > :" << endl;
        cin >> cpu;
        if (cpu < 0)
        {
            cout << "Wrong cpu core count " << cpu << "." << endl;
            exit(1);
        }
        cluster[labname] = cpu;
    }

    Message M;
    int i = 0;
    map<string, int>::iterator iter;
    for (iter = cluster.begin(); iter != cluster.end(); iter++)
    {
        M.msg["NODES"][i] = iter->first;
        M.msg["NPS"][i] = iter->second;
        M.msg["ONPS"][i] = iter->second;
        i++;
    }
    M.encode_Header("server", "scheduler", "nodestate");

    Message queue;
    int job, neednode, neednp;

    cout << "Enter the job count. < Example : 10 > :" << endl;
    cin >> job;

    if (job < 0)
    {
        cout << "Wrong job count " << job << "." << endl;
        exit(1);
    }
    for (int i = 0; i < job; i++)
    {
        queue.msg["JOBID"][i] = i;

        cout << "Enter the "
             << "JOB" << i << "'s  NODE need. < Example : 3 > :" << endl;
        cin >> neednode;
        if (neednode < 0)
        {
            cout << "Wrong NODE need " << neednode << "." << endl;
            exit(1);
        }
        cout << "Enter the "
             << "JOB" << i << "'s  CPU need. < Example : 3 > :" << endl;
        cin >> neednp;
        if (neednp < 0)
        {
            cout << "Wrong CPU need " << neednp << "." << endl;
            exit(1);
        }

        queue.msg["NODENEED"][i] = neednode;
        queue.msg["NPNEED"][i] = neednp;
    }

    queue.encode_Header("server", "scheduler", "queuestate");

    cout << "nodestate is :" << endl
         << M.encode_Message() << endl;
    cout << "queuestate is :" << endl
         << queue.encode_Message() << endl;

    cout << "Enter 0 to exit , Enter 1 to send." << endl;
    int check;
    cin >> check;
    if (check == 0)
        exit(1);

    while (queue.msg["JOBID"].size() > 0)
    {
        Message do_sch;
        do_sch.encode_Header("server", "scheduler", "do_schedule");
        do_sch.msg["IP"] = "127.0.0.1";
        do_sch.msg["PORT"] = "5004";

        cc_socket cs;
        cs.setConnection("127.0.0.1", "5002");
        cs.connect2server();
        cs.send(do_sch.encode_Message());
        cs.closeConnection();

        s_socket ss;
        ss.setConnection("127.0.0.1", "5004");
        ss.acceptClinet();

        string request = ss.readmessage();

        M.initMessage();
        int i = 0;
        map<string, int>::iterator iter;
        for (iter = cluster.begin(); iter != cluster.end(); iter++)
        {
            M.msg["NODES"][i] = iter->first;
            M.msg["NPS"][i] = iter->second;
            M.msg["ONPS"][i] = iter->second;
            i++;
        }

        M.encode_Header("server", "scheduler", "nodestate");
        ss.sendmessage(M.encode_Message());

        request = ss.readmessage();

        ss.sendmessage(queue.encode_Message());

        cout << "The result is : " << endl;
        cout << request << endl;

        request = ss.readmessage();
        json runjob = json::parse(request);
        for (int y = 0; y < (int)runjob["JOBID"].size(); y++)
        {
            queue.msg["JOBID"].erase(0);
            queue.msg["NODENEED"].erase(0);
            queue.msg["NPNEED"].erase(0);
        }
        ss.closeConnection();
        ss.closebind();
    }
}