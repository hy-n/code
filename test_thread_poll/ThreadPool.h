#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include <thread>
#include <queue>
#include <iostream>
#include <vector>
#include <mutex>
#include <map>
#include <future>
#include <condition_variable>
#include <functional>
#include <thread>
#include <sstream>
using namespace std;
#define IF_OPEN_AUTO_DEL_THREADS 1
#define IF_OPEN_AUTO_ADD_THREADS 1
#define MIN_THREAD_POOL_SIZE 2
#define MAX_THREAD_POOL_SIZE 16

class ThreadPool {
public:
  ThreadPool(int size);
  ~ThreadPool();
  typedef std::function<void()> TaskType;

  void getTask();
  void push2TaskQueue(TaskType task);

  // template <typename F, typename... Args>
  // void push2TaskQueueWithPara(F &&f, Args &&...args);

template<typename F, typename... Args>
auto push2TaskQueueWithPara(F&& f, Args&&... args) 
    -> std::future<typename std::invoke_result<F, Args...>::type>
{
    //prepare task(package task)
    using task_return_type =  typename std::invoke_result<F, Args...>::type;
    auto func        = std::make_shared<std::packaged_task<task_return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    auto lambda_func = [func]() { 
        cout << "in lambda func before do task" << endl;
        (*func)();
    };

    //add task into queue
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _taskQueue.emplace(lambda_func);
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
    return func->get_future();

    //-------------automatically unlocked-------------
}


  void start();
  void updateThreadMap();

private:
  // std::vector<std::thread*> _threads;
  std::map<std::string, std::thread *> _threads;
  std::queue<TaskType> _taskQueue;
  std::queue<std::string> delThreadIdQueue;
  std::mutex _mutex;
  std::condition_variable _cond;
  int _thread_poll_size;
  bool if_start;
  std::thread *monitor_thread;
  unsigned int _min_thread_size;
};

#endif