#include <iostream>
#include <string>
#include <vector>
#include "Processor.h"
using namespace std;
class Job:public JobInterface
{
public:
    int a,b;
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

int main(int argc, char *argv[])
{
    int fd = ManageProcessor<Job>::createManager("main",3);
    std::string str = "3 10 25 4 5 12 34";
    write(fd,str.c_str(),str.size()+1);
    char buf[100];
    read(fd,buf,100);
    std::cout << buf << std::endl;
    return 0;
}

