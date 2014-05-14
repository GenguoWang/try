#include "HttpClient.h"
#include <iostream>
#include <map>
using namespace std;
void callback(const string & content)
{
    cout << "result:" << endl;
    cout << content << endl;
}
int main(int argc,char *argv[])
{
    HttpClient hc;
    map<string,string> kv;
    kv[argv[2]] = argv[3];
    cout << "hhh" << endl;
    hc.asyncPost(argv[1],kv,callback);
    hc.run();
    return 0;
}
