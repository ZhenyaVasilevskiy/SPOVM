#include <iostream>
#include <signal.h>
#include <list>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

pid_t last=0;

void signal_free(int signo);

int main()
{
    struct sigaction act;                                                                               ///структура с обработчиком
    act.sa_handler=signal_free;                                                                         ///передать в структура указатель на обработчик
    sigaction(SIGUSR1,&act,NULL);                                                                       ///установить обработчик на сигнал sigusr1 (пользовательски)
    list<pid_t> list_of_threads;                                                                        ///лист хендлеров
    pid_t pproc=getpid();                                                                               ///пид родительского
    pid_t tmp_pid;
    char command;
    do
    {
        command = 0;
        cin>>command;
        cin.clear();
        if(command=='+')
        {
           for(list<pid_t>::iterator i=list_of_threads.begin(); i!= list_of_threads.end();i++)
                kill(*i,SIGSTOP);                                                                       ///послать стоп сигнал всем процессам
            switch(tmp_pid=fork())
            {
            case -1:
                cout<<("Thread creating error\n");
                break;
            case 0:
            {
                pid_t prev_thread=list_of_threads.front();///получить пид предыдущего процесса
                int thread_num=list_of_threads.size()+1;///узнать номер процесса
                char str[6];
                str[0]='S';str[1]='t';str[2]='r';str[3]='i';str[4]='n';str[5]='g';
                while(1)
                {
                    for(int i=0;i<6;i++)
                    {
                        usleep(10000);
                        cout<<str[i];
                    }
                    cout<<thread_num<<endl;
                    sleep(thread_num*0.3+1);///каждый процесс ожидает различное время
                    if(thread_num!=1)
                    {
                        kill(prev_thread,SIGCONT);///послать предыдущему процкссу сигнал о продолжении
                        raise(SIGSTOP);///послать себе стоп сигнал
                    }
                    else
                    {
                        kill(pproc,SIGUSR1);///если номер процесса 1, послать родительскому пользовательский сигнал 1
                        raise(SIGSTOP);///послать самому себе тсоп сигнал
                    }
                }
                exit(77);
            }
            break;
            default:
                {
                    last=tmp_pid;
                    list_of_threads.push_front(tmp_pid);///родительский процесс записывает в лист пид созданного процесса
                }
            }

        }
        if(command=='-')
        {
            kill(list_of_threads.front(), SIGKILL);/// верхний процесс в листе останавливается
            list_of_threads.pop_front();
            last=list_of_threads.front();
            kill(last,SIGCONT);///посылает последнему процессу сигнал о продолжении
        }
    }while(command!='q');
    while(list_of_threads.size())///завершить все процессы
    {
        kill(list_of_threads.front(), SIGKILL);
        list_of_threads.pop_front();
    }
    return 0;
}

void signal_free(int signo)///обработчик SIGUSR1
{
    kill(last,SIGCONT);///посылает сигнал о продолжении последнему процессу
}
