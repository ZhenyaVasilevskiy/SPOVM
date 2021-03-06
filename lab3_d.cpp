﻿// lab3_d.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

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
	HANDLE in_file;
	output = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, TRUE, "lab3TransferredString");
	execute = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, TRUE, "lab3Execute");
	if (!output)
		return 0;
	//string string_for_output="empty_string\n";
	in_file=CreateFile(TEXT("\\\\.\\pipe\\transfer"), GENERIC_READ||GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (in_file == INVALID_HANDLE_VALUE)
		cout << "programm can't open the file";
	do
	{
		WaitForSingleObject(output, INFINITE);
		char buffer[1000];
		ReadFile(in_file, buffer, 1000, NULL, NULL);
		cout << buffer << endl;
		//cout << string_for_output;
	} while (ReleaseSemaphore(execute,1,NULL)==FALSE);
	CloseHandle(in_file);
	return 0;
}

