#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
using namespace std;
int i = 0;
void func() { 
    cout << "here in task func, i is " << i++ << endl; 
}

int main() { 
    ThreadPool poll(5);
    cout << "after create thread pool~" << endl;
    //1. test for 5 threads with 1 task
    for (int i = 0; i < 500;i++){
        poll.push2TaskQueue(&func);
    }
      
    sleep(20);
    // poll.~ThreadPool();
    // sleep(20);
}