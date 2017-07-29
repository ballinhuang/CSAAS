#include"Node.hpp"
#include<iostream>

using namespace std;


Node::Node(){
    // null
}

Node::Node(string ip, string port, string name, int core){
    setnodeip(ip);
    setnodeport(port);
    setnodename(name);
    setCPUcore(core);
    original_CPU_core = core;
}

string Node::getnodeip(){
    return nodeip;
}

string Node::getnodeport(){
    return nodeport;
}

string Node::getnodename(){
    return nodename;
}

int Node::getnodeCPUcore(){
    return CPU_core;
}

int Node::getoriginalCPUcore(){
    return original_CPU_core;
}

void Node::setnodeip(string ip){
    nodeip = ip;
}

void Node::setnodeport(string port){
    nodeport = port;
}

void Node::setnodename(string name){
    nodename = name;
}

void Node::setCPUcore(int core){
    CPU_core = core;
}