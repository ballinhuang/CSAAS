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

void Node::setnodeip(string ip){
    nodeip = ip;
}

void Node::setnodeport(string port){
    nodeport = port;
}

void Node::setnodename(string name){
    nodename = name;
}
