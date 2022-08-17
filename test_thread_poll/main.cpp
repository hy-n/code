#include "ThreadPool.h"
#include <iostream>
#include <unistd.h>
#include <map>
using namespace std;
int i = 0;
void func() { 
    cout << "here in task func, i is " << i++ << endl; 
}
// g++ main.cpp ThreadPool.cpp -o pro -std=c++17 -pthread
int func_with_para(int taskId, int para2){
    cout << "in func para, taskId:" << taskId << " para2:" << para2 << endl;
    return taskId * para2;
}

void func_with_para_no_return_val(int taskId, int para2, int para3){
    cout << "in func_with_para_no_return_val, taskId:" << taskId << " para2:" << para2 << " para3:" << para3 << endl;
}

int main() { 
    ThreadPool poll(5);
    // std::vector<std::future<int>> results;
    std::map<int, std::future<int>> results;


    cout << "after create thread pool~" << endl;
    //1. test for 5 threads with 1 task
    for (int i = 0; i < 500;i++){
        cout << "add task Id:" << i << endl;
        if(i%3 == 0){ 
            poll.push2TaskQueueWithPara(&func);
        } else if(i%5==0){
            poll.push2TaskQueueWithPara(&func_with_para_no_return_val, i, 2, 3);
        } else{
            results[i] = poll.push2TaskQueueWithPara(&func_with_para, i, 2);
        }
        
        // poll.push2TaskQueue(&func);
    }

    sleep(5);

    std::map<int, std::future<int>>::iterator iter1;
    for (iter1 = results.begin();iter1 != results.end();iter1++)
	{
        std::cout << "result of task " << iter1->first << " : " << iter1->second.get() << std::endl;
    }


    sleep(20);
    // poll.~ThreadPool();
    // sleep(20);
}