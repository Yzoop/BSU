#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#pragma warning(disable: 4996)

int small_file_size_default = (1 << 15);
#define intsz sizeof(int)
#define splited_file_name ".tmp"
#define INPUT_FILE "input.txt"
#define OUTPUT_FILE "output.txt"

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

int split(const char* input) {
	std::ifstream fin(input);

	int big_file_size = 0;
	fin >> big_file_size;

	int small_file_count = big_file_size / small_file_size_default;
	char* name = new char[20];
	int number;
	for (int i = 0; i < small_file_count; ++i) {
		sprintf(name, "%d%s", i, splited_file_name);
		int* arr = new int[small_file_size_default];
		for (int i = 0; i < small_file_size_default; ++i) {
			fin >> number;
			arr[i] = number;
		}
		FILE* out = fopen(name, "wb");
		fwrite(&small_file_size_default, intsz, 1, out);
		sort(arr, small_file_size_default);
		fwrite(arr, intsz, small_file_size_default, out);
		fclose(out);
	}
	int count = (big_file_size % small_file_size_default);
	if (count) {
		sprintf(name, "%d%s", small_file_count, splited_file_name);
		int* arr = new int[count];
		for (int i = 0; i < count; ++i) {
			fin >> number;
			arr[i] = number;
		}
		FILE* out = fopen(name, "wb");
		fwrite(&count, intsz, 1, out);
		sort(arr, count);
		fwrite(arr, intsz, count, out);
		fclose(out);
	}

	fin.close();
	printf("Dividing finished\n");
	free(name);
	if (count)
		return small_file_count + 1;
	return small_file_count;
}

void merge_files(char* name1, char* name2, char* nameout) {
	FILE *in1, *in2, *mout;
	int small_file_size1, small_file_size2;
	in1 = fopen(name1, "rb");
	in2 = fopen(name2, "rb");
	mout = fopen(nameout, "wb");
	int number1, number2;
	fread(&small_file_size1, intsz, 1, in1);
	fread(&small_file_size2, intsz, 1, in2);
	fread(&number1, intsz, 1, in1);
	fread(&number2, intsz, 1, in2);
	int count = small_file_size1 + small_file_size2;
	fwrite(&count, sizeof(int), 1, mout);
	int peek1 = small_file_size1 - 1;
	int peek2 = small_file_size2 - 1;
	for (int i = 0; i < count; ++i) {
		if (!peek1) {
			fwrite(&number2, sizeof(int), 1, mout);
			fread(&number2, sizeof(int), 1, in2);
			continue;
		}
		if (!peek2) {
			fwrite(&number1, sizeof(int), 1, mout);
			fread(&number1, sizeof(int), 1, in1);
			continue;
		}
		if (number1 > number2) {
			fwrite(&number2, sizeof(int), 1, mout);
			fread(&number2, sizeof(int), 1, in2);
			peek2--;
		}
		else {
			fwrite(&number1, sizeof(int), 1, mout);
			fread(&number1, sizeof(int), 1, in1);
			peek1--;
		}
	}
	fclose(in1);
	fclose(in2);
	fclose(mout);
}

void merge(int small_file_count) {
	int merge = small_file_count;
	char* name1 = new char[20];
	char* name2 = new char[20];
	char* nameout = new char[20];
	while (merge != 1) {
		for (int i = 0; i < merge / 2; ++i) {
			sprintf(name1, "%d.tmp", 2 * i);
			sprintf(name2, "%d.tmp", 2 * i + 1);
			sprintf(nameout, "m%d.tmp", i);
			merge_files(name1, name2, nameout);
			remove(name1);
			remove(name2);
			sprintf(name1, "%d.tmp", i);
			rename(nameout, name1);
		}
		if (merge % 2) {
			sprintf(name1, "%d.tmp", merge / 2);
			sprintf(name2, "%d.tmp", merge);
			sprintf(nameout, "tail.tmp");
			merge_files(name1, name2, nameout);
			remove(name1);
			remove(name2);
			sprintf(name1, "%d.tmp", merge / 2);
			rename(nameout, name1);
		}
		merge /= 2;
	}
	free(name1);
	free(name2);
	free(nameout);
}

void to_nornal_txt(const char* output) {
	FILE* out = fopen(output, "w");
	FILE* in = fopen("0.tmp", "rb");
	int count;
	fread(&count, intsz, 1, in);
	int number;
	fprintf(out, "%d\n", count);
	for (int i = 0; i < count; ++i) {
		fread(&number, intsz, 1, in);
		fprintf(out, "%d ", number);
	}
	fclose(out);
	fclose(in);
	remove("0.tmp");
}

int main() {
	int start = clock();

	int splited_files_count = split(INPUT_FILE);
	merge(splited_files_count);
	to_nornal_txt(OUTPUT_FILE);

	start = clock() - start;
	printf("Elapsed time: %d ms.\n", start);
}