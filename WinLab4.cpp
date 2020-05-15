#include <iostream>
#include <conio.h>
#include <Windows.h>
#include <stack>
#include <string>

CRITICAL_SECTION cs;

int threadNumber;

DWORD WINAPI printThread(void*) {
	threadNumber++;
	std::string thread = std::to_string(threadNumber) + " thread";
	while (1) {
		EnterCriticalSection(&cs);
		for (int i = 0; i < thread.size(); i++) {
			std::cout << thread[i];
		}
		std::cout << std::endl;
		LeaveCriticalSection(&cs);
		Sleep(200);
	}
}

int main() {
	std::stack<HANDLE> threadStack;
	InitializeCriticalSection(&cs);
	std::cout << "+ - create thread\n- - terminate thread\nq - terminate all threads\n";
	while (1) {
		fflush(stdin);
		switch (_getch()) {
			case '+':
				threadStack.push(CreateThread(NULL,NULL,&printThread,NULL,NULL,NULL));
				break;
			case '-' :
				if (!threadStack.empty()) {
					TerminateThread(threadStack.top(),0);
					CloseHandle(threadStack.top());
					threadStack.pop();
				}
				if (threadStack.empty()) {
					std::cout << "No active threads, type + to create a new one" << std::endl;
				}
				break;
			case 'q' :
				while (!threadStack.empty()) {
					TerminateThread(threadStack.top(), 0);
					CloseHandle(threadStack.top());
					threadStack.pop();
				}
				DeleteCriticalSection(&cs);
				return 0;
		}
	}
}
