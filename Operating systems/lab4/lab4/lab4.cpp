// lab4.cpp: ���������� ����� ����� ��� ����������� ����������.
//

#include <iostream>
#include <windows.h>
#include "SyncHeap.h"
using namespace std;

CRITICAL_SECTION csConsole;

struct parameters
{
	SyncHeap* heap;
	int threadIndex;
	HANDLE hReadyEvent;
	HANDLE hBeginEvent;
};

int printMenu(int threadNumber) {
	EnterCriticalSection(&csConsole);
	cout << "\n-----Thread number " << threadNumber << "---------\n";
	cout << "[1] - allocate\n";
	cout << "[2] - free\n";
	cout << "[3] - continue\n";
	cout << "[4] - exit\n" << endl;
	int n;
	cin >> n;
	LeaveCriticalSection(&csConsole);
	return n;
}

DWORD WINAPI thread(LPVOID param)
{
	parameters* pr = (parameters*)param;


	SetEvent(pr->hReadyEvent);
	WaitForSingleObject(pr->hBeginEvent, INFINITE);


	int choose = -1;
	while (choose != 4) {
		choose = printMenu(pr->threadIndex);
		switch (choose) {
		case 1:
			int sizeOfBlock;
			EnterCriticalSection(&csConsole);
			cout << "Enter the size of block: ";
			cin >> sizeOfBlock;
			LeaveCriticalSection(&csConsole);
			pr->heap->allocate(sizeOfBlock, pr->threadIndex);
			Sleep(7);
			pr->heap->print();
			break;
		case 2:
			pr->heap->free(pr->threadIndex);
			Sleep(7);
			pr->heap->print();
			break;
		}
	}
	return 0;
}

int main()
{
	InitializeCriticalSection(&csConsole);
	int  n;
	cout << "Enter heap size: ";
	cin >> n;
	SyncHeap arr(n);

	int users;
	cout << "Enter count of users: ";
	cin >> users;

	HANDLE* hThread = new HANDLE[users];
	DWORD* ThreadID = new DWORD[users];
	HANDLE* hReadyEvent = new HANDLE[users];
	HANDLE hBeginEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	parameters* pr = new parameters[users];

	for (int i = 0; i < users; ++i) {
		pr[i].threadIndex = i + 1;
		pr[i].heap = &arr;
		pr[i].hReadyEvent = hReadyEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		pr[i].hBeginEvent = hBeginEvent;
		hThread[i] = CreateThread(NULL, 0, thread, (void*)&pr[i], 0, &ThreadID[i]);
	}
	WaitForMultipleObjects(users, hReadyEvent, TRUE, INFINITE);
	SetEvent(hBeginEvent);

	WaitForMultipleObjects(users, hThread, TRUE, INFINITE);
	cout << "All ready";


	//CloseHandle(hBeginEvent);
	for (int i = 0; i < users; ++i) {
		CloseHandle(hThread[i]);
		//CloseHandle(hReadyEvent[i]);
	}
	delete[] hThread;
	delete[] ThreadID;
	//delete[] hReadyEvent;
	delete[] pr;
	DeleteCriticalSection(&csConsole);
	return 0;
}


