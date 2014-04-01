#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <queue>
#define NTHREADS 4
#define NUM_THREADS 5
#define N 1000
#define MEGEXTRA 1000000
 
using namespace std;
pthread_attr_t attr;
 
 pthread_mutex_t mutexsum;
class ThreadPool;
struct ThreadData
{
    queue<int> * q;
    int t;
    void (*dowork)(void *);
};
void *threadWork(void *data)
{
   ThreadData *td = (ThreadData*)data;
   cout << "tid" << td->t << endl;
   bool hasValue = false;
   int a;
    pthread_mutex_lock (&mutexsum);
    printf("q address tid%d :%x\n",td->t,td->q);
   if(!td->q->empty())
   { 
        a = td->q->front();
        td->q->pop();
        hasValue = true;
        cout << "get:" << a << endl;
   }
   pthread_mutex_unlock (&mutexsum);
   //if(hasValue) td->dowork((void*)a);
   
   pthread_exit(NULL);
}
class ThreadPool
{
public:
    queue<int> task;
    pthread_t threads[NUM_THREADS];
    void (*dowork)(void *);
    void start()
    {
        int rc;
        for(int t=0;t<NUM_THREADS;t++)
        {
            ThreadData *td = new ThreadData;
            td->t = t;
            td->q = &task;
            td->dowork = dowork;
              rc = pthread_create(&threads[t], NULL, threadWork, (void *)td);
              //sleep(1);
              if (rc){
                 printf("ERROR; return code from pthread_create() is %d\n", rc);
                 exit(-1);
              }
        }
    }
};

void todo(void * num)
{
    cout << "work:" << (long)num << endl;
}
int main(int argc, char *argv[])
{
pthread_mutex_init(&mutexsum, NULL);
    ThreadPool tp;
    tp.dowork = todo;
    tp.task.push(1);
    tp.task.push(2);
    tp.task.push(3);
    tp.task.push(4);
    tp.task.push(5);
    tp.start();
   // pthread_mutex_destroy(&mutexsum);
   pthread_exit(NULL);
}
