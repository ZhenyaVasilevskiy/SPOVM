#include <iostream>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

sem_t* canPrint;
sem_t* buffer;

int main(int argc,char** argv) {
    sem_unlink("buffer");
    sem_unlink("canPrint");
    int pipeFiles[2];
    if(pipe(pipeFiles) != 0) {
        std::cout << "Error creating pipe." << std::endl;
        return 1;
    }
    buffer = sem_open("buffer",O_CREAT,0644,1);
    canPrint = sem_open("canPrint",O_CREAT,0644,0);
    std::string toClient = "";
    switch (fork()) {
        case -1:
            std::cout << "Error creating process" << std::endl;
            exit(1);
        case 0:
            char buff[80];
            while(1) {
                sem_wait(canPrint);
                memset(&buff,0,80);
                read(pipeFiles[0],buff,79);
                std::string fromServer = buff;
                if(fromServer == "stop") {
                    exit(0);
                }
                std::cout << "Client : " << fromServer << std::endl;
                sem_post(buffer);
            }

        default:
            do {
                sem_wait(buffer);
                std::cout << "Server : ";
                std::cin >> toClient;
                write(pipeFiles[1],toClient.c_str(),toClient.length());
                sem_post(canPrint);
            } while(toClient != "stop");
    }
    sem_close(buffer);
    sem_close(canPrint);
    sem_unlink("buffer");
    sem_unlink("canPrint");
    return 0;
}
