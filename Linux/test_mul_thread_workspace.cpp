#include<iostream>
#include <thread>
#include <chrono>
#include<unistd.h>
#include<string.h>
#include<dlfcn.h>
using namespace std;
//compile cmd
// g++ -g test_mul_thread_workspace.cpp -std=c++11 -o pro -pthread


//conclusion:
//getcwd()获取到的工作路径并不准确，可能被其他线程更改，
//使用readlink()获取的是准确的?

void thread_func() 
{
  sleep(5);
  char buf[100];
  char buf2[100];
  getcwd(buf, 100);
  readlink("/proc/self/exe", buf2, 100);

  cout << "in child thread, workspace from getcwd:" << buf << endl;
  cout << "in child thread, workspace from getcwd:" << buf2 << endl;

  std::cout << "child thread end." << std::endl;
}

int main() 
{
  std::thread t1(thread_func);
  t1.detach();
  sleep(2);
  chdir("/root/study");
  cout << "in main thread" << endl;
  sleep(50);
  // t1.join(); 
}