#include<iostream>
#include<string.h>
#include<stdio.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<unistd.h>

#define SHM_KEY 100
#define SEM_KEY 100
using namespace std;

// struct sembuf {
//         unsigned short  sem_num;        /* semaphore index in array */
//         short           sem_op;         /* semaphore operation */
//         short           sem_flg;        /* operation flags */
// };

// union semun{
//     int val;        // used for SETVAL only
//     struct semid_ds *buf; // used for IPC_SET and IPC_STAT
//     ushort *array;        // used for GETALL and SETALL
// };
union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
     short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO
								(Linux-specific) */
};


void processRead(){
    //initialize shm
    int shm_id = shmget(SHM_KEY, 40, IPC_CREAT );
    cout << "in processRead, shm_id:" << shm_id << endl;
    char* shm_ptr = (char*)shmat(shm_id, nullptr, IPC_CREAT);
    perror("semctl failed.\n");

     
    printf("shm content before writer do sth: %s\n", shm_ptr);

    int sem_id = semget(SEM_KEY, 1, IPC_CREAT );
    sembuf val_sembuf[1];
    val_sembuf[0].sem_op = -1;
    val_sembuf[0].sem_num =0;
    val_sembuf[0].sem_flg=SEM_UNDO;
    cout<<"in processRead, before semop"<<endl;
    semop(sem_id, val_sembuf, 1);
    cout<<"in processRead, after semop"<<endl;
    //read content
    printf("shm content after writer do sth: %s\n", shm_ptr);
    val_sembuf[0].sem_op = 1;
    semop(sem_id, val_sembuf, 1);

    semctl(sem_id, 0, IPC_RMID);
    shmdt(shm_ptr);
    shmctl(shm_id, IPC_RMID, nullptr);

}



void processWrite(){
    //initialize shm
    int shm_id = shmget(SHM_KEY, 40, IPC_CREAT | IPC_EXCL);

    if(shm_id < 0){
        cout << "create shm failed" <<endl;
    }
    int sum_time_s = 30;
    char* shm_ptr = (char*)shmat(shm_id, nullptr, IPC_CREAT | IPC_EXCL);

    //initialize sem
    int sem_id = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL);

    semun val_semun;
    val_semun.val = 1;
    if(semctl(sem_id, 0, SETVAL, val_semun)!=0){
        perror("semctl failed.\n");
        return;
    }

    struct sembuf val_sembuf[1];
    val_sembuf[0].sem_op = -1;
    val_sembuf[0].sem_num =0;
    val_sembuf[0].sem_flg=SEM_UNDO;
    //write to shm with sem to protect

    //hold resources
    cout<<"before write, hold resources"<<endl;
    try{
        if(semop(sem_id, val_sembuf, 1) < 0){
            perror("semop failed.\n");
            exit(-1);
        }
    }
    catch(...){
        exit(-1);
    }
    cout <<"before while loop"<<endl;
    int i=0;
    while(i< sum_time_s){
        char temp = 'A' + i;
        shm_ptr[i] = temp;
        cout <<"i="<< i<<endl;
        i++;
        sleep(1);
    }

    //release resources
    val_sembuf[0].sem_op = 1;
    semop(sem_id, val_sembuf, 1);
    cout<<"after write, release resources"<<endl;

}

int main(){
    pid_t pid;
    cout <<"val of IPC_CREAT:" << IPC_CREAT <<endl;
    cout <<"val of IPC_EXCL:" << IPC_EXCL <<endl;
    cout<<"main process id:"<<getpid()<<endl;
    if( ( pid = fork() ) == 0){ //in child process
        processWrite();
        cout<<"before process_write exit..."<<endl;
        return 0;
    }
    sleep(1);
    
    cout <<"before read process..."<<endl;
    if( ( pid = fork() ) == 0){ //in child process
        cout <<"before read process in if()..."<<endl;
        processRead();
        cout<<"before process_read exit..."<<endl;
        return 0;
    }
    
    cout << "main process exit..."<<endl;
    return 0;
    
}