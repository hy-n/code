#include<sys/shm.h>
#include<unistd.h>
#include<iostream>
#include<string.h>

#define SHM_KEY 111

using namespace std;


void process1Func(){ //write
    auto shm_id_1 = shmget(SHM_KEY, 20, IPC_CREAT);
    cout << "shm_id in process1:" << shm_id_1 << endl;

    auto ptr_1 = shmat(shm_id_1, nullptr, IPC_CREAT);
    cout << "addr1 in process1:" << ptr_1<<endl;
    
    char buf[19]="hello world";
    memcpy(ptr_1, buf, 19);

}


void process2Func(){ //read
    auto shm_id_2 = shmget(SHM_KEY, 20, IPC_CREAT);
    cout << "shm_id in process2:" << shm_id_2 << endl;

    auto ptr_2 = shmat(shm_id_2, nullptr, IPC_CREAT);
    cout << "addr2 in process2:" << ptr_2 <<endl;

    char buf[19]={"0"};
    memcpy(buf, ptr_2, 19);
    cout << "content in shared memory:" << buf <<endl;

    shmctl(shm_id_2, IPC_RMID, nullptr);
}


int main()
{
    pid_t pid;
    cout<<"main process id:"<<getpid()<<endl;
    if( ( pid = fork() ) == 0){ //in child process
        process1Func();
        return 0;
    }
    cout << "process1 id:"<<pid<<endl;
    cout<<"in main process, child process1 finished" << endl;
    sleep(3);
     if( (pid = fork() ) == 0){ //in child process
        process2Func();
        return 0;
    }
    sleep(3);
    cout << "process2 id:"<<pid<<endl;
    cout << "before main process returned."<<endl;
    return 0;
}