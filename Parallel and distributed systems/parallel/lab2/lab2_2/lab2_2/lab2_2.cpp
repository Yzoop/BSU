// lab2_2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"

using namespace std;

int** matrix;
int* vectB;
int* vectResult;
int matrixSize;

struct matrix_rows {
	int start;
	int step;
	int count;
};
DWORD WINAPI thread_func(LPVOID param) {
	matrix_rows* mr = (matrix_rows*)param;
	for (int i = 0; i < mr->count; ++i) {
		int index = mr->start + i * mr->step;
		for (int j = 0; j < matrixSize; ++j)
			vectResult[index] += matrix[index][j] * vectB[j];
	}
	return 0;
}

int main() {
	LARGE_INTEGER liFrequency, liStartTime, liFinishTime;
	int type;

	cout << "Enter matrix size: ";
	cin >> matrixSize;

	matrix = new int*[matrixSize];
	vectB = new int[matrixSize];
	vectResult = new int[matrixSize];

	for (int i = 0; i < matrixSize; ++i) {
		vectB[i] = rand();
		matrix[i] = new int[matrixSize];
		for (int j = 0; j < matrixSize; ++j)
			matrix[i][j] = rand();
	}

	while (true) {
		cout << "\nEnter type:\n1 - linear\n2 - continuous\n3 - cyclic\nany button to exit\n";
		cin.clear();
		cin >> type;
		for (int i = 0; i < matrixSize; ++i)
			vectResult[i] = 0;

		QueryPerformanceFrequency(&liFrequency);
		if (type == 1) {
			QueryPerformanceCounter(&liStartTime);
			for (int i = 0; i < matrixSize; ++i) {
				for (int j = 0; j < matrixSize; ++j)
					vectResult[i] += matrix[i][j] * vectB[j];
			}

			QueryPerformanceCounter(&liFinishTime);
			double dElapsedTime = 1000.*(liFinishTime.QuadPart - liStartTime.QuadPart) / liFrequency.QuadPart;
			cout << "Time: " << dElapsedTime;
		}
		else if (type == 2 || type == 3) {
			int threadNumber;
			cout << "Enter number of threads: ";
			cin >> threadNumber;

			HANDLE* hThread = new HANDLE[threadNumber];
			matrix_rows* param = new matrix_rows[threadNumber];

			QueryPerformanceCounter(&liStartTime);
			if (type == 2)
				for (int i = 0; i < threadNumber; ++i) {
					param[i].step = 1;
					param[i].count = matrixSize / threadNumber + ((i < (matrixSize % threadNumber)) ? 1 : 0);
					param[i].start = (i == 0 ? 0 : (param[i - 1].start + param[i - 1].count));
				}
			else
				for (int i = 0; i < threadNumber; ++i) {
					param[i].step = threadNumber;
					param[i].count = matrixSize / threadNumber + ((i < (matrixSize % threadNumber)) ? 1 : 0);
					param[i].start = i;
				}

			for (int i = 0; i < threadNumber; ++i)
				hThread[i] = CreateThread(NULL, 0, thread_func, (LPVOID)(param + i), 0, NULL);
			WaitForMultipleObjects(threadNumber, hThread, TRUE, INFINITE);

			QueryPerformanceCounter(&liFinishTime);
			double dElapsedTime = 1000.*(liFinishTime.QuadPart - liStartTime.QuadPart) / liFrequency.QuadPart;
			cout << "Time: " << dElapsedTime;

			for (int i = 0; i < threadNumber; ++i)
				CloseHandle(hThread[i]);
			delete[] hThread;
			delete[] param;
		}
		else
			break;
	}

	for (int i = 0; i < matrixSize; ++i)
		delete[] matrix[i];
	delete matrix;
	delete[] vectB;
	delete[] vectResult;
}

