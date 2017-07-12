#include"Node.hpp"
#include<iostream>

using namespace std;


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