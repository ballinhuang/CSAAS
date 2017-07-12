#ifndef _NODE
#define _NODE

#include<iostream>

class Node{
    public:
    std::string getnodeip();
    std::string getnodeport();
    std::string getnodename();
    int getnodeCPUcore();

    void setnodeip(std::string);
    void setnodeport(std::string);
    void setnodename(std::string);
    void setCPUcore(int);

    private:
    std::string nodeip,nodeport,nodename;
    int CPU_core;
};

#endif