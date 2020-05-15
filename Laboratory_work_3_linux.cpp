#include <iostream>
#include <signal.h>
#include <list>
#include <string>
#include <sstream>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <pthread.h>
#include <errno.h>
#include <termios.h>
#include <sys/shm.h>
#include <sys/ipc.h>



using namespace std;

int CreateSemaphore(const char* path, int proj_id);
int WaitForSemaphore(int sem_id);
int CheckSemaphore(int sem_id);
int ReleaseSemaphore(int sem_id);
int IncrementSemaphore(int sem_id);

int main(int argc, char** argv)
{
    pid_t client;  ///дескриптор дочернего процесса
    volatile int sem_id_out=CreateSemaphore(argv[0],1), sem_id_exit=CreateSemaphore(argv[0],2); ///создать 2 семафора для разрешения вывода и разрешения работы дочернего процесса
    int pipe_file[2];///0 - read; 1 - write
    int pipe_id=pipe(pipe_file);
    switch(client=fork())///создание дочернего процесса
    {
    case -1: ///если не создан
        cout<<("Thread creating error\n");
        break;
    case 0:
    {
        close(pipe_file[1]);///закрыть write
        int i =300;
        cout<<"Child process: has been started\n";
        char str_buf[1000];
        while(CheckSemaphore(sem_id_exit)!=0)
        {
            cout<<"\nChild process: ";
            WaitForSemaphore(sem_id_out);
            if(CheckSemaphore(sem_id_exit)==0) ///если семафор разрешения работы == 0
                break;
            char str_buf[1000];
            read(pipe_file[0],str_buf,1000);
            cout<<str_buf;
            IncrementSemaphore(sem_id_out);
            i--;
        };
        cout<<"has been closed\n";
        exit(77);
    }
    break;
    default:
        {
            close(pipe_file[0]);///закрыть read
            do
            {
                string transferred_str;
                ///string exit_string = "q";
                ///cout<<"enter a string\n";
                getline(cin, transferred_str, '\n');
                if (transferred_str == "end")
                {
                    write(pipe_file[1],transferred_str.c_str(),transferred_str.size()+1);
                    ReleaseSemaphore(sem_id_exit);
                    ReleaseSemaphore(sem_id_out);
                    break;
                }
                write(pipe_file[1],transferred_str.c_str(),transferred_str.size()+1);

                ReleaseSemaphore(sem_id_out);
                cin.clear();
            } while (1);
            ///ReleaseSemaphore(sem_id_out);
            sleep(0.1);
        }
    }

    return 0;
}


int CreateSemaphore(const char* path, int proj_id)
{
    int sem_id;

    sem_id = ftok(path, proj_id);
    sem_id = semget(IPC_PRIVATE, 1, IPC_CREAT|0666);
    semctl(sem_id, 0, SETVAL, 1);
    return sem_id;
}


int WaitForSemaphore(int sem_id)//ожидание свободной критической секции
{
    struct sembuf wait_buf;

    wait_buf.sem_num = 0;
    wait_buf.sem_op = 0;
    wait_buf.sem_flg=SEM_UNDO;
    return semop(sem_id, &wait_buf, 1);
}

int IncrementSemaphore(int sem_id) //увелиение значения счетчика,дргуие процессы понимают что критическая секция занята
{
    struct sembuf increment_buf;
    increment_buf.sem_flg=SEM_UNDO;
    increment_buf.sem_num = 0;
    increment_buf.sem_op = 1;
    return semop(sem_id, &increment_buf, 1);
}

int CheckSemaphore(int sem_id) //провериь значение семафора
{
    struct sembuf wait_buf;

    wait_buf.sem_flg=IPC_NOWAIT;
    wait_buf.sem_num = 0;
    wait_buf.sem_op = 0;
    return semop(sem_id, &wait_buf, 1);
}

int ReleaseSemaphore(int sem_id) //декремент счетчика
{
    struct sembuf decrement_buf;
    decrement_buf.sem_flg=SEM_UNDO;
    decrement_buf.sem_num = 0;
    decrement_buf.sem_op = -1;
    return semop(sem_id, &decrement_buf, 1);
}
