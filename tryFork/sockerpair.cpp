#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <iostream>
using namespace std;
int main()
{
    int fd[2];
    pid_t pid;
    socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
    if((pid = fork()) == 0)
    {
        //child
        close(fd[0]);
        char buf[100];
        int num = read(fd[1],buf,100);
        string data(buf,num);
        cout << data.length() << endl;
        cout << "read: " << num << " " << data << endl;
    }
    else
    {
        close(fd[1]);
        write(fd[0],"hello",6);
    }
    return 0;
}
