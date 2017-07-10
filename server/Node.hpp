#ifndef _NODE
#define _NODE

#include<iostream>

class Node{
    public:
    std::string getnodeip();
    std::string getnodeport();
    std::string getnodename();
    
    void setnodeip(std::string);
    void setnodeport(std::string);
    void setnodename(std::string);
    
    private:
    std::string nodeip,nodeport,nodename;
};

#endif