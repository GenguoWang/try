#include "HttpClient.h"
#include <iostream>
#include <map>
using namespace std;
int main(int argc,char *argv[])
{
    HttpClient hc;
    map<string,string> kv;
    kv[argv[2]] = argv[3];
    cout << "hhh" << endl;
    cout << hc.post(argv[1],kv) <<endl;
    return 0;
}
