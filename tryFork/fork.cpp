#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
void usePipe()
{
    int pipeFd[2];
    if(pipe(pipeFd)<0)
    {
        cout << "pipe create error" << endl;
        return;
    }
    int pid;
    if((pid=fork())==0)
    {
        //child
        close(pipeFd[1]);
        close(pipeFd[1]);
        char buf[100];
        int num;
        if((num=read(pipeFd[0],buf,100))>0)
        {
            buf[num] = '\0';
            cout << "child read: " <<buf << endl;
        }
        else
        {
            cout << "child read failed" << endl;
        }
        //write(pipeFd[1],"Child",5);
    }
    else if(pid > 0)
    {
        //parent
        close(pipeFd[0]);
        close(pipeFd[0]);
        if(write(pipeFd[1],"Hello",5)!=-1)
        {
            cout << "parent write success" << endl;
        }
        sleep(2);
        int num;
        char buf[100];
        //num = read(pipeFd[0],buf,100);
        //buf[num] = '\0';
        //cout << "parent read: " << buf << endl;
        waitpid(pid,NULL,0);
    }
    else
    {
        //error
        cout << "fork error" << endl;
    }
}
int createChild(void (*f)())
{
    int pid = fork();
    if(pid==-1)
    {
        return -1;
    }
    else if(pid==0)
    {
        //child
        f();
        exit(0);
    }
    else
    {
        return pid;
    }
}
int main()
{
    usePipe();
    return 0;
}
