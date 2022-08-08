#include "ThreadPool.h"
#include "unistd.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;
#define MIN_THREAD_POOL_SIZE 5
ThreadPool::ThreadPool(int size) : _thread_poll_size(size) {
  if_start = true;
  start();
  monitor_thread == new std::thread(std::bind(&ThreadPool::updateThreadMap, this));

}

ThreadPool::~ThreadPool(){
    // std::unique_lock<std::mutex> lock(_mutex);
    if_start = false;
    _cond.notify_all();
    
    for (int i = 0; i < _threads.size(); i++){
      _threads[i]->join();
      cout << "after thread join, thread No." << i << endl;
    }

    monitor_thread->join();

    // task queue empty?
}

void ThreadPool::updateThreadMap(){
    while(if_start){
      std::unique_lock<std::mutex> lock(_mutex);
      while(!delThreadIdQueue.empty()){
        _threads.erase(delThreadIdQueue.front());
        delThreadIdQueue.pop();
      }
      sleep(1);
    }
}

void ThreadPool::getTask(){
    while(1){
        std::unique_lock<std::mutex> lock(_mutex);
        while(if_start &&  _taskQueue.empty()){
            cout << "before cond wait~" << endl;
            _cond.wait(lock);
        }
        if(!_taskQueue.empty()){
            auto task = _taskQueue.front();
            _taskQueue.pop();
            task();
            // if thread num > task num, thread finished 
            if((_taskQueue.size() > _threads.size()) &&  (_threads.size() > MIN_THREAD_POOL_SIZE)){
              std::stringstream sstream;
              sstream << std::this_thread::get_id();  //?thread中获取的id和std::thread::get_id是一致的吗？
              delThreadIdQueue.push(atoi(sstream.str().c_str()));
              break;
            }
        }
        if(!if_start){
            return;
        }
        cout << "after one time of task running, before sleep(1)" << endl;
        sleep(1);
    }
}

void ThreadPool::push2TaskQueue(TaskType task){
    std::unique_lock<std::mutex> lock(_mutex);
    _taskQueue.emplace(task);
    _cond.notify_one();

    //double thread num if task_count > 5*thread_count
    if(_taskQueue.size() > 5*_threads.size()){
      for (int i = 0; i < _threads.size(); i++){
        auto t = new std::thread(std::bind(&ThreadPool::getTask, this)); 
        std::stringstream sstream;
        sstream << t->get_id();
        _threads[atoi(sstream.str().c_str())]  = t;
      }
    }

}

void ThreadPool::start(){
    for(int i=0; i<_thread_poll_size; i++){
        auto t = new std::thread(std::bind(&ThreadPool::getTask, this)); 
        std::stringstream sstream;
        sstream << t->get_id();
        _threads[atoi(sstream.str().c_str())]  = t;
    }

    
}