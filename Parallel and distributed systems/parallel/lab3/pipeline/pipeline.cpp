// pipeline.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <queue>
#include <fstream>
#include "Windows.h"

using namespace std;

int small_file_array_length_default = 1000;

void sort(int* arr, int n) {
	int i, j;
	for (i = 0; i < n - 1; i++)
		for (j = 0; j < n - i - 1; j++)
			if (arr[j] > arr[j + 1]) {
				int buf = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = buf;
			}
}

void merge_files(char* name1, char* name2, char* nameout) {
	fstream in1(name1);
	fstream in2(name2);
	ofstream out(nameout);
	int file1_size, file2_size;
	int number1, number2;
	in1 >> file1_size;
	in2 >> file2_size;
	in1 >> number1;
	in2 >> number2;
	int count = file1_size + file2_size;
	out << count << "\n";
	int peek1 = file1_size;
	int peek2 = file2_size;
	for (int i = 0; i < count; ++i) {
		if (!peek1) {
			out << number2 << " ";
			in2 >> number2;
			continue;
		}
		if (!peek2) {
			out << number1 << " ";
			in1 >> number1;
			continue;
		}
		if (number1 > number2) {
			out << number2 << " ";
			in2 >> number2;
			peek2--;
		}
		else {
			out << number1 << " ";
			in1 >> number1;
			peek1--;
		}
	}
	in1.close();
	in2.close();
	out.close();
}

CRITICAL_SECTION cs_que;
queue<int> que_buf;
HANDLE semaphore;

void insert_file_in_queue(int i) {
	EnterCriticalSection(&cs_que);
	que_buf.push(i);
	ReleaseSemaphore(semaphore, 1, NULL);
	LeaveCriticalSection(&cs_que);
}

int get_file_from_queue() {
	WaitForSingleObject(semaphore, INFINITE);
	EnterCriticalSection(&cs_que);
	int result = que_buf.front();
	que_buf.pop();
	LeaveCriticalSection(&cs_que);
	return result;
}

DWORD WINAPI thread_fnc_sorting(LPVOID data) {
	char * inputFile = (char*)data;
	fstream in(inputFile);
	int array_length;
	in >> array_length;
	int small_files_count = array_length / small_file_array_length_default;
	char* name = new char[20];
	int number;
	for (int i = 0; i < small_files_count; ++i) {
		sprintf(name, "%d.tmp", i);
		int* arr = new int[small_file_array_length_default];
		for (int j = 0; j < small_file_array_length_default; ++j) {
			in >> arr[j];
		}
		ofstream out(name);
		out << small_file_array_length_default << "\n";
		sort(arr, small_file_array_length_default);
		for (int j = 0; j < small_file_array_length_default; j++) {
			out << arr[j] << " ";
		}
		out.close();
		insert_file_in_queue(i);
	}
	int tail = (array_length % small_file_array_length_default);
	if (tail) {
		sprintf(name, "%d.tmp", small_files_count);
		small_files_count++;
		int* arr = new int[tail];
		for (int i = 0; i < tail; ++i) {
			in >> arr[i];
		}
		ofstream out(name);
		out << tail << '\n';
		sort(arr, tail);
		for (int j = 0; j < tail; j++) {
			out << arr[j] << " ";
		}
		out.close();
		insert_file_in_queue(small_files_count - 1);
	}
	in.close();
	printf("Sorting finished\n");
	insert_file_in_queue(-1);
	free(name);
	return 0;
}

DWORD WINAPI thread_fnc_merging(LPVOID data) {
	int to_merge = 0;
	char* name1 = new char[20];
	char* name2 = new char[20];
	char* nameout = new char[20];
	while (true) {
		int file1 = get_file_from_queue();
		if (file1 != -1) {
			sprintf(name1, "%d.tmp", file1);
			sprintf(nameout, "m%d.tmp", to_merge);
			to_merge++;
			int file2 = get_file_from_queue();
			if (file2 != -1) {
				sprintf(name2, "%d.tmp", file2);
				merge_files(name1, name2, nameout);
				remove(name1);
				remove(name2);
			}
			else {
				rename(name1, nameout);
				break;
			}
		}
		else {
			break;
		}
	}

	while (to_merge != 1) {
		for (int i = 0; i < to_merge / 2; ++i) {
			sprintf(name1, "m%d.tmp", 2 * i);
			sprintf(name2, "m%d.tmp", 2 * i + 1);
			sprintf(nameout, "mm%d.tmp", i);
			merge_files(name1, name2, nameout);
			remove(name1);
			remove(name2);
			sprintf(name1, "m%d.tmp", i);
			rename(nameout, name1);
		}
		if (to_merge % 2) {
			sprintf(name1, "m%d.tmp", (to_merge / 2) - 1);
			sprintf(name2, "m%d.tmp", to_merge - 1);
			sprintf(nameout, "tail.tmp");
			merge_files(name1, name2, nameout);
			remove(name1);
			remove(name2);
			sprintf(name1, "m%d.tmp", (to_merge / 2) - 1);
			rename(nameout, name1);
		}
		to_merge /= 2;
	}
	remove("output.txt");
	rename("m0.tmp", "output.txt");
	printf("Merging finished\n");
	free(name1);
	free(name2);
	free(nameout);
	return 0;
}

int main() {
	LARGE_INTEGER liFrequency, liStartTime, liFinishTime;
	// �������� �������
	QueryPerformanceFrequency(&liFrequency);
	// �������� ��������� �����  
	QueryPerformanceCounter(&liStartTime);
	InitializeCriticalSection(&cs_que);
	HANDLE sortingThread = CreateThread(NULL, 0, thread_fnc_sorting, (void*)"input.txt", NULL, NULL);
	HANDLE mergingThread = CreateThread(NULL, 0, thread_fnc_merging, NULL, NULL, NULL);;
	semaphore = CreateSemaphore(NULL, 0, 0xffffff, NULL);

	WaitForSingleObject(sortingThread, INFINITE);
	WaitForSingleObject(mergingThread, INFINITE);
	CloseHandle(sortingThread);
	CloseHandle(mergingThread);
	CloseHandle(semaphore);

	DeleteCriticalSection(&cs_que);
	QueryPerformanceCounter(&liFinishTime);
	double dElapsedTime = 1000.*(liFinishTime.QuadPart - liStartTime.QuadPart) / liFrequency.QuadPart;
	printf("Time = %lf\n", dElapsedTime);
	system("pause");
	return 0;
}

