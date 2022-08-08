#include<thread>
#include<iostream>
#include<string>
#include<sstream>
#include<unistd.h>
using namespace std;

void func() 
{
    std::stringstream sstream;
    sstream << std::this_thread::get_id();
    std::string thread_no = sstream.str();
    
    cout << "hi thread num in thread:" << thread_no << endl; 
}

int main()
{
    thread t1(func);

    std::stringstream sstream;
    sstream << t1.get_id();
    std::string thread_no = sstream.str();

    cout << "thread id by get_id()" << thread_no << endl;
    sleep(100);
}