// Reporter.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>

using namespace std;

struct student
{
	char name[10]; // ��� ��������
	int num; // ����� ������
	double grade; // ������� ����
};

int main(int argc, char* argv[])
{
	fstream binin(argv[0], ios::in | ios:: binary);
	ofstream fout(argv[1]);
	float average = atof(argv[2]);

	fout << "����� �� ����� " << argv[0] << "\n";
	fout << "������ ��������� �� ������� ����� ���� " << argv[2] << "\n";
	student s;
	while (binin.read((char*)&s, sizeof(struct student))) {
		if (average < s.grade) {
			fout << "Name: " << s.name << "\n";
			fout << "Group number: " << s.num << "\n";
			fout << "Grade: " << s.grade << "\n";
			fout << endl;
		}
	}
	fout.close();
	binin.close();


	system("pause");
    return 0;
}

