#include"Message.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;
// for convenience
using json = nlohmann::json;

void Message::initMessage(){
    msg.clear();
}

string Message::encode_Message(){
    string result = msg.dump();
    return result;
}

void Message::encode_Header(string sender, string receiver, string request){
    msg["SENDER"] = sender;
    msg["RECEIVER"] = receiver;
    msg["REQUEST"] = request;
}
