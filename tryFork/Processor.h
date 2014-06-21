#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_
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

class Processor
{
protected:
    int mFd;
    FILE *mFp;
    std::string name;
public:
    Processor(int fd):mFd(fd)
    {
        mFp = fdopen(mFd,"rw");
    }
    Processor(int fd,std::string name):mFd(fd),name(name)
    {
        mFp = fdopen(mFd,"rw");
    }
    void setName(std::string name)
    {
        this->name = name;
    }
    virtual void run()
    {
        std::string input;
        while(true)
        {
            int inputState = acceptInput(input);
            if(input.length()>0)
            {
                std::string result = work(input);
                outputResult(result);
            }
            if(inputState == EOF)
            {
                break;   
            }
        }
    }
    virtual int acceptInput(std::string &input)
    {
        std::ostringstream out;
        int buf;
        while((buf = fgetc(mFp))!=EOF && buf != '\0')
        {
            out << (char)buf;
        }
        input = out.str();
        return buf;
    }
    virtual std::string work(std::string input)=0;
    virtual void outputResult(std::string result)
    {
        write(mFd, result.c_str(), result.size()+1);// size()+1 to output the '\0'
    }
    virtual ~Processor()
    {
        close(mFd);
    }
};


template<typename JobType>
class WorkerProcessor:public Processor
{
public:
    WorkerProcessor(int fd):Processor(fd){}
    WorkerProcessor(int fd,std::string name):Processor(fd,name){}
    std::string work(std::string input)
    {
        std::cout << "worker " << name <<": " << input << std::endl;
        std::istringstream in(input);
        std::ostringstream out;
        int size,a,b;
        in >> size;
        JobType job;
        bool isFirst = true;
        for(int i=0; i<size; ++i)
        {
            job.readFromStream(in);
            if(isFirst)
            {
                isFirst = false;

            }
            else
            {
                out << " ";
            }
            out << job.run();
        }
        return out.str();
    }
    static int createWorker(std::string name)
    {
        int fd[2];
        pid_t pid;
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        if((pid = fork()) == 0)
        {
            close(fd[0]);
            WorkerProcessor<JobType> worker(fd[1],name);
            worker.run();
            exit(0);
        }
        else if(pid>0)
        {
            close(fd[1]);
            return fd[0];
        }
        else
        {
            throw std::runtime_error("fork failed");
        }
    }
};


template<typename JobType>
class ManageProcessor:public Processor
{
private:
    std::vector<int> workerFds;
public:
    ManageProcessor(int fd):Processor(fd){}
    ManageProcessor(int fd,std::string name):Processor(fd,name){}
    void addWorker(int fd)
    {
        workerFds.push_back(fd);
    }
    std::string work(std::string input)
    {
        int workerNum = workerFds.size();
        if(workerNum==0) 
        {
            throw std::runtime_error("No worker is added");
        }
        std::cout << "manager " << name << ": " << input<<std::endl;
        std::cout << "worker num: " << workerNum<<std::endl;
        std::vector<int> workingFd;
        std::istringstream in(input);
        std::ostringstream out;
        int size,a,b;
        in >> size;
        if(size == 0) return std::string();
        std::vector<JobType> jobList;
        for(int i=0; i<size; ++i)
        {
            JobType job;
            job.readFromStream(in);
            jobList.push_back(job);
        }
        int neededWorker = std::min(workerNum,size);
        int oneJobNum = (size-1)/neededWorker + 1;
        for(int index=0; index < neededWorker; index++)
        {
            int num = std::min(oneJobNum, size - index*oneJobNum);
            std::ostringstream oWork;
            oWork << num;
            for(int i=0; i < num;++i)
            {
                oWork << " ";
                jobList[index*oneJobNum+i].writeToStream(oWork);
            }
            std::string workStr = oWork.str();
            write(workerFds[index],workStr.c_str(),workStr.size()+1);
        }
        char buf[1000];
        bool isFirst = true;
        for(int index=0; index < neededWorker; index++)
        {
            read(workerFds[index],buf,1000);
            if(isFirst)
            {
                isFirst = false;
            }
            else
            {
                out << " ";
            }
            out << buf;
        }
        return out.str();
    }
    static int createManager(std::string name, int workerNum)
    {
        int fd[2];
        pid_t pid;
        socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
        if((pid = fork()) == 0)
        {
            close(fd[0]);
            ManageProcessor<JobType> manager(fd[1],name);
            for(int i=0; i < workerNum; ++i)
            {
                int workerFd = WorkerProcessor<JobType>::createWorker(name +":"+std::string("work")+(char)('0'+i));
                manager.addWorker(workerFd);
            }
            manager.run();
            exit(0);
        }
        else if(pid > 0)
        {
            close(fd[1]);
            return fd[0];
        }
        else
        {
            throw std::runtime_error("fork failed");
        }
    }
};

class JobInterface
{
public:
    virtual void readFromStream(std::istream &in)=0;
    virtual void writeToStream(std::ostream &out)=0;
    virtual std::string run()=0;
};
#endif
