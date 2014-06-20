#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdexcept>
using namespace std;
struct Job
{
    int a,b;
    void readFromStream(istream &in)
    {
        in >> a >> b;
    }
    void writeToStream(ostream &out)
    {
        out << a << " " << b;
    }
    string run()
    {
        ostringstream oStr;
        oStr << a+b;
        return oStr.str();
    }
}
class Processor
{
protected:
    int mFd;
    FILE *mFp;
public:
    Processor(int fd):mFd(fd)
    {
        mFp = fdopen(mFd,"rw");
    }
    virtual void run();
    virtual int acceptInput(std::string &input);
    virtual std::string work(std::string input)=0;
    virtual void outputResult(std::string);
};
void Processor::run()
{
    string input;
    while(true)
    {
        int inputState = acceptInput(input);
        if(input.length()>0)
        {
            string result = work(input);
            outputResult(result);
        }
        if(inputState == EOF)
        {
            cout << "EOF" << endl;
            break;   
        }
    }
}
int Processor::acceptInput(string &input)
{
    ostringstream out;
    int buf;
    while((buf = fgetc(mFp))!=EOF && buf != '\0')
    {
        out << (char)buf;
    }
    input = out.str();
    return buf;
}
void Processor::outputResult(string result)
{
    write(mFd, result.c_str(), result.size()+1);// size()+1 to output the '\0'
}


class WorkerProcessor:public Processor
{
public:
    WorkerProcessor(int pfd):Processor(pfd){}
    std::string work(std::string input);
};

string WorkerProcessor::work(string input)
{
    istringstream in(input);
    ostringstream out;
    int size,a,b;
    in >> size;
    out << size;
    for(int i=0; i<size; ++i)
    {
        in >> a >> b;
        out << " "<< a+b;
    }
    return out.str();
}

class ManageProcessor:Processor
{
private:
    vector<int> workerFds;
public:
    ManageProcessor(int fd):Processor(fd){}
    void addWorker(int fd);
    std::string work(std::string input);
};
void ManageProcessor::addWorker(int fd)
{
    workerFds.push_back(fd);
}
string ManageProcessor::work(string input)
{
    int workerNum = workerFds.size();
    if(workerNum==0) 
    {
        throw runtime_error("No worker is added");
    }
    vector<int> workingFd;
    istringstream in(input);
    ostringstream out;
    int size,a,b;
    in >> size;
    if(size == 0) return string();
    int oneJobNum = size / workerNum;
    for(int i=0; i<size; ++i)
    {
        in >> a >> b;
        out << " "<< a+b;
    }
    out << size;
    return out.str();
    
}

int createWorker()
{
    int fd[2];
    pid_t pid;
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if((pid = fork()) == 0)
    {
        //child
        close(fd[0]);
        WorkerProcessor worker(fd[1]);
        worker.run();
        exit(0);
    }
    else
    {
        close(fd[1]);
        return fd[0];
    }
}
int main()
{
    int fd = createWorker();
    string str = "2 10 25 4 5";
    write(fd,str.c_str(),str.size()+1);
    char buf[100];
    read(fd,buf,100);
    cout << buf << endl;
    close(fd);
    wait(NULL);
    return 0;
}
