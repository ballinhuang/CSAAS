#include"job.hpp"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>

using namespace std;
// for convenience
using json = nlohmann::json;

void job::initjob(){
    attribute.clear();
}

string job::encode_job(){
    string result = attribute.dump();
    return result;
}

void job::addattirbute(string a,string b){
    attribute.emplace(a,b);
}