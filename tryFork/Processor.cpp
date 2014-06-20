#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <string>
#include <iostream>
#include <sstream>
using namespace std;
class Processor
{
protected:
    int fd;
    FILE *fp;
public:
    Processor(int fd):fd(fd)
    {
        fp = fdopen(fd,"rw");
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
    while((buf = fgetc(fp))!=EOF && buf != '\0')
    {
        out << (char)buf;
    }
    input = out.str();
    return buf;
}
void Processor::outputResult(string result)
{
    write(fd, result.c_str(), result.size()+1);// size()+1 to output the '\0'
}


class WorkerProcessor:public Processor
{
public:
    WorkerProcessor(int fd):Processor(fd){}
    std::string work(std::string input);
};

string WorkerProcessor::work(string input)
{
    istringstream in(input);
    ostringstream out;
    int a,b;
    in >> a >> b;
    out << a+b;
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
    string str = "10 25";
    write(fd,str.c_str(),str.size()+1);
    char buf[100];
    read(fd,buf,100);
    cout << buf << endl;
    close(fd);
    wait(NULL);
    return 0;
}
