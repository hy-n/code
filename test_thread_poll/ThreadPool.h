#ifndef __THREADPOOL_H
#define __THREADPOOL_H

#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <map>
#include <condition_variable>

class ThreadPool{
public:
  ThreadPool(int size);
  ~ThreadPool();
  typedef std::function<void()> TaskType;

  void getTask();
  void push2TaskQueue(TaskType task);
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