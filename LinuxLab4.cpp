#include <iostream>
#include <pthread.h>
#include <stack>
#include <unistd.h>

pthread_mutex_t mutex;
int thread;

void* printThread(void*) {
    thread++;
    std::string threadNumber = std::to_string(thread) + " thread";
    while (1) {
        pthread_mutex_lock(&mutex);
        for(int i = 0;i < threadNumber.size();i++) {
            std::cout << threadNumber[i];
        }
        std::cout << std::endl;
        pthread_mutex_unlock(&mutex);
        usleep(200000);
    }
}

int main() {
    pthread_t threadId = 0;
    std::stack<pthread_t> threadsStack;
    char branch;
    std::cout << "+ - create thread\n- - terminate last thread\nq - terminate all threads\n";
    pthread_mutex_init(&mutex,NULL);
    while(1) {
        std::cin.clear();
        std::cin >> branch;
        switch(branch) {
            case '+' :
                pthread_create(&threadId,NULL,printThread,NULL);
                threadsStack.push(threadId);
                threadId++;
                break;

            case '-' :
                if(!threadsStack.empty()) {
                    pthread_cancel(threadsStack.top());
                    threadsStack.pop();
                }
                if(threadsStack.empty()) {
                    std::cout << "No active threads, type + to create a new one" << std::endl;
                }
                break;

            case 'q' :
                while(!threadsStack.empty()) {
                    pthread_cancel(threadsStack.top());
                    threadsStack.pop();
                }
                pthread_mutex_destroy(&mutex);
                return 0;

            default :
                std::cout << "Unknown command" << std::endl;
        }
    }
}
