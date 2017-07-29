#ifndef _NODE
#define _NODE

#include<iostream>

class Node{
    public:
    Node();
    Node(std::string, std::string, std::string, int);
    std::string getnodeip();
    std::string getnodeport();
    std::string getnodename();
    int getnodeCPUcore();
    int getoriginalCPUcore();

    void setnodeip(std::string);
    void setnodeport(std::string);
    void setnodename(std::string);
    void setCPUcore(int);

    private:
    std::string nodeip,nodeport,nodename;
    int CPU_core;
    int original_CPU_core;
};

#endif