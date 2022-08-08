#include "ThreadPool.h"


#include "unistd.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>

using namespace std;
#define MIN_THREAD_POOL_SIZE 2
#define MAX_THREAD_POOL_SIZE 16

#define IF_OPEN_AUTO_DEL_THREADS 1
#define IF_OPEN_AUTO_ADD_THREADS 1

ThreadPool::ThreadPool(int size) : _thread_poll_size(size) {
  if_start = true;
  cout << "hardware concurrency count:" << std::thread::hardware_concurrency() << endl;
  _min_thread_size = (std::thread::hardware_concurrency() == 0) ? 2 : std::thread::hardware_concurrency();
  start();

  // if(IF_OPEN_AUTO_DEL_THREADS){
  //   monitor_thread == new std::thread(std::bind(&ThreadPool::updateThreadMap, this));
  // }
}

ThreadPool::~ThreadPool(){
    // std::unique_lock<std::mutex> lock(_mutex);
    if_start = false;
    _cond.notify_all();
    int i = 0;

    for (auto iter = _threads.begin(); iter != _threads.end(); iter++){
        iter->second->join();
        cout << "after thread join, thread No." << i++ << endl;
    }

    // if(IF_OPEN_AUTO_DEL_THREADS){
    //   monitor_thread->join();
    // }
    // task queue empty?
}

void ThreadPool::updateThreadMap(){
    while(if_start){
      std::unique_lock<std::mutex> lock(_mutex);
      if(!delThreadIdQueue.empty()){
          auto thread_id_to_del = delThreadIdQueue.front();
          cout << "del thread from _threads, _threads size now:" << _threads.size() << " delThreadIdQueue size:" << delThreadIdQueue.size() << "thread id to del from map:" << thread_id_to_del << endl;
          _threads.erase(thread_id_to_del);
          cout << "thread size after erase:" << _threads.size() << endl;
          delThreadIdQueue.pop();
      }  
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
            std::stringstream sstream;
            sstream << std::this_thread::get_id();
            auto current_thread_id = sstream.str();
            cout << "running thread id:" << sstream.str() << endl;
            task();

            if(IF_OPEN_AUTO_DEL_THREADS){
              // if thread num > task num, thread finished
              if ((_taskQueue.size() < _threads.size()) && (_threads.size() > _min_thread_size))
              {
                  cout << "add to delThreadIdQueue, _taskQueue size:" << _taskQueue.size()
                  << " _threads size:" << _threads.size() << " const v size:" << _min_thread_size << " thread id to terminate:" << current_thread_id << endl;

                  std::stringstream sstream;
                  sstream << std::this_thread::get_id();  
                  // delThreadIdQueue.push(sstream.str());
                  _threads.erase(current_thread_id);
                  return;
              }
            }      
        }

        if(!if_start){
            return;
        }
        cout << "after one time of task running, before sleep(1)" << endl;
        // sleep(1);
    }
}

void ThreadPool::push2TaskQueue(TaskType task){
    std::unique_lock<std::mutex> lock(_mutex);
    _taskQueue.emplace(task);
    _cond.notify_one();

    if(IF_OPEN_AUTO_ADD_THREADS){
      //double thread num if task_count > 5*thread_count
      if(_taskQueue.size() > 5*_threads.size() && _threads.size() < MAX_THREAD_POOL_SIZE){
        
        auto t = new std::thread(std::bind(&ThreadPool::getTask, this));
        std::stringstream sstream;
        sstream << t->get_id();
        cout << "add thread num, current thread size:" << _threads.size() << " new thread id:" << sstream.str() << endl;
        _threads[sstream.str()] = t;
      }
    }
}

void ThreadPool::start(){
    for(int i=0; i<_thread_poll_size; i++){
        auto t = new std::thread(std::bind(&ThreadPool::getTask, this)); 
        std::stringstream sstream;
        sstream << t->get_id();
        _threads[sstream.str()]  = t;
    }

    
}