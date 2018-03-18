#pragma once
#include <iostream>
#include <windows.h>

class SyncHeap
{
private:
	int* heap;
	int size;
	CRITICAL_SECTION csConsole;
public:
	SyncHeap(int nSize);            // nSize - ������ ����
	~SyncHeap();

	int* allocate(int nElement);    // �������� �������� �� ����
	void free(int* nPtr);           // ������� �������� � ����
	void print();                   // �������� ������, ��� �������
};

