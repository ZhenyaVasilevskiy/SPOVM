// lab3.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <process.h>
#include <time.h>
#include <list>
#include <sstream>
#include <string.h>
#include <windows.h>

using namespace std;


int main()
{
	HANDLE output;
	HANDLE execute;
	HANDLE client_proc;
	HANDLE out_file;
	output = CreateSemaphoreA(NULL, 0, 2, "lab3TransferredString");  //функция создания семафора вывода (атрибуты защиты, чему равен, максимальное значение,имя)
	execute= CreateSemaphoreA(NULL, 1, 1, "lab3Execute");  //функция создания семафора выполнения (атрибуты защиты, чему равен, максимальное значение,имя)
	STARTUPINFO startupinfo;
	ZeroMemory(&startupinfo, sizeof(startupinfo));  //обнуление startupinfo
	PROCESS_INFORMATION procinfo;
	ZeroMemory(&procinfo, sizeof(procinfo));   //обнуление procinfo
	TCHAR Com_line[] = TEXT("lab3_d");  //текст для командной строки
	//LPCWSTR name = TEXT("lab2_d");
	startupinfo.cb = sizeof(startupinfo);
	if (!CreateProcess(NULL, Com_line, NULL, NULL, NULL, CREATE_NEW_CONSOLE, NULL, NULL, &startupinfo, &procinfo)) //создаем дочерний процесс
		std::cout << "Process creating error\n";  //если процесс не был создан
	client_proc = procinfo.hProcess;   //передать в client_proc дескриптор дочернего процесса
	LPCWSTR file_name = TEXT("\\\\.\\pipe\\transfer");   //имя канала (\\имя сервера\\pipe\\Имя канала)
	out_file=CreateNamedPipeW(file_name, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE|PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 1000, 1000, 5000, NULL); //создать именованный канал
	// с параметрами (имя канала, режим открытия канала, максимальное количество реализаций канала,размер выходного буфера, размер входного буфера,время ожидания,атибуты защиты)
	ConnectNamedPipe(out_file, NULL);  //соединить канал с дочерним процессом
	do
	{
		string transferred_str;
		//string exit_string = "q";
		getline(cin, transferred_str, '\n');
		if (transferred_str == "q")
			break;
		WriteFile(out_file, transferred_str.c_str(), transferred_str.size()+1, NULL, NULL); //передать значение строки в дочерний процесс
		//где параметры это (дескриптор файла (в нашем случае канала),буфер данных,число байтов для записи,число записанных байтов,асинхонный буфер)
		ReleaseSemaphore(output, 1, NULL);  //увеличение значения счетчика семафора (идентификатор семафора,значение увеличения,адрес переменной для записи предыдущего значения семафора)
		cin.clear();
	} while (1);
	char empty[] = "";
	WriteFile(out_file, empty, 1, NULL, NULL);
	WaitForSingleObject(execute,10);
	ReleaseSemaphore(output, 1, NULL);
	CloseHandle(client_proc);
	CloseHandle(output);
	CloseHandle(out_file);
}




