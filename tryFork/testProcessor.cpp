#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include "Processor.h"
#include <cassert>
using namespace std;
class JobAdd:public JobInterface
{
public:
    int a,b;
    JobAdd(){}
    JobAdd(int a,int b):a(a),b(b){}
    virtual void readFromStream(std::istream &in)
    {
        in >> a >> b;
    }
    virtual void writeToStream(std::ostream &out)
    {
        out << a << " " << b;
    }
    std::string run()
    {
        std::ostringstream oStr;
        oStr << a+b;
        return oStr.str();
    }
};

void testAdd()
{
    int fd = ManageProcessor<JobAdd>::createManager("main",5);
    vector<JobAdd> toTest;
    vector<int> result;
    srand(time(NULL));
    int num = rand()%100;
    for(int i=0; i < num; ++i)
    {
        int a = rand()%65536;
        int b = rand()%65536;
        toTest.push_back(JobAdd(a,b));
        result.push_back(a+b);
    }
    ostringstream oStr;
    oStr << num;
    for(int i=0; i < num; ++i)
    {
        oStr << " ";
        toTest[i].writeToStream(oStr);
    }
    string str = oStr.str();
    write(fd,str.c_str(),str.size()+1);
    char buf[1000];
    read(fd,buf,1000);
    string res(buf);
    istringstream iStr(res);
    int temp;
    bool isPassed = true;
    for(int i=0; i < num; ++i)
    {
        iStr >> temp;
        if(temp != result[i])
        {
            isPassed = false;
            cerr << "Error"<<endl;
            toTest[i].writeToStream(cerr);
            cerr << endl << "reuslt:" << temp << endl;
            cerr << "expect: " << result[i] << endl;
        }
        assert(temp==result[i]);
    }
    cout << (isPassed?"Passed":"Failed") << endl;
}
int main(int argc, char *argv[])
{
    testAdd();
    return 0;
}

