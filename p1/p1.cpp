// p1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <fstream>
#include<iostream>
#include<thread>
//#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include<list>
#include<string>
#include<time.h>

using namespace std;

int m=0;
int threadsNum;
int checkNum;
int lastRow = 0;
bool allScanFinish = false;
bool allDel = false;
bool allCheckFinish=false;
clock_t* finish;
int* allTimes;

class Coord {
public:
	Coord(int x, int y) : x(x), y(y) {};

	int x;
	int y;
};

class RowStatus{
public:
	RowStatus(int n, bool scan, bool check, vector<int>* row, RowStatus* next) : n(n), scan(scan), check(check), row(row), next(next) {};
	RowStatus(int n, vector<int>* row, RowStatus* next) : n(n), scan(false), check(false), row(row), next(next) {};
	RowStatus(int n, vector<int>* row) : n(n), scan(false), check(false), row(row), next(NULL) {};
	RowStatus() :n(0), scan(false), check(false), row(NULL), next(NULL) {};

	vector<int>* requireCheck() {
		if (check == false && scan == true){ 
			check = true; scan = false; return row;}
		else { return NULL; }
	}

	vector<int>* requireDelete() {
		if (check = true && scan == true && next->check == true && next->scan == true) return row;
		else return NULL;
	}

	int n;
	bool scan;
	bool check;
	RowStatus* next;

private:
	vector<int>* row;
};

class CheckResult {
public: 
	CheckResult() : isThreadFinish(false) {};

	bool isThreadFinish;
	vector<Coord> peaks;
};

RowStatus* tail = NULL;
CheckResult* result = NULL;

void scan(int row, int col) {
	int t;
	RowStatus* tempRow = NULL;
	for (int i = 0; i < row; i++) {
		vector<int>* v = new vector<int>;
		RowStatus* r = new RowStatus{i, v};
		for (int j = 0; j < col; j++) {
			cin >> t;
			v->push_back(t);
		}
		if (tempRow == NULL) {
			tempRow = r;
			tail = r;
		}
		else {
			tempRow->next = r;
			tempRow = r;
		}
		r->scan = true;
		r->check = false;
		lastRow = i;
	}
	allScanFinish = true;
	//system("exit");
}

void check(int i, CheckResult* result, int row, int col) {
	RowStatus* temp = NULL;

	while (tail == NULL) {
	}
	while (temp != tail) {
		temp = tail;
	}
	while (true) {
		if (temp->check==false && temp->scan==true) {
			vector<int>* tempVec = temp->requireCheck();
			if(tempVec != NULL){
				for (int k = 1, bool slope=NULL; k < tempVec->size(); k++) {
					if (tempVec->at(k) >= tempVec->at(k - 1)) {
						slope = true;
					}
					else { slope = false; }

				}
				temp->check = true;
				temp->scan = true;
			}else {
				
			}
			
		}
		else if (temp->check == false && temp->scan == false) {

		}
		else {
			if (temp->n != row - 1) { 
				while (temp->next == NULL) {  }
				temp = temp->next; }
			else break;
		}
	}
	result->isThreadFinish = true;
}

void del() {
	RowStatus* temp = tail;
	while (true) {
		if (temp->check == true && temp->scan == true && temp->next->check==true && temp->next->scan==true) {
			vector<int>* vec = temp->requireDelete();
			if (vec != NULL) {
				RowStatus* delRow = temp;
				if (temp->n != lastRow || allScanFinish != true) {
					temp = temp->next;
					tail = temp;
				}
				else {
					tail = NULL;
					allDel = true;
				}
				delete(vec);
				delete(delRow);
			}
			
		}
	}
	//system("exit");
}

int main(int argc, char* argv[]) {
	/*FILE* fin;
	fopen_s(&fin, "matrix.dada", "r");
	fscanf_s(fin, "%d", &row);
	fscanf_s(fin, "%d", &col);*/
	
		threadsNum = thread::hardware_concurrency();
		checkNum = threadsNum - 2;
		result = new CheckResult[threadsNum];
		finish = new clock_t[threadsNum];
		allTimes = new int[threadsNum];
		int row, col, buffer[2];
	
		cin >> row;
		cin >> col;
		
		thread scanThread(scan, row, col);
		scanThread.detach();
		for (int i = 0; i < checkNum; i++) {
			thread checkThread(check, i, result+i, row, col);
			checkThread.detach();
		}

		bool p = true;
		while (p) {
			RowStatus* temp = tail;
			p = false;
			for (int i = 0; i < row; i++) {
				cout << temp->n << ". " << temp->check << " " << temp->scan << endl;
				if (!(temp->check == true && temp->scan == true))p = true;
				if (temp->next != NULL) { temp = temp->next; }
				else break;
			}
			cout << "-----------------------------------" << endl;

		}

		while (true) {
			if (allScanFinish) {
				cout << "Scan Finish" << endl;
				break;
			}
		}


		bool f = true;
		while (f) {
			f = false;
			for (int i = 0; i < checkNum; i++) {
				if (!result[i].isThreadFinish) { f = true; }
			}
		}





		/*string path("C:\MyOwnData\Coding\Data Structure Project1 Find Peak\Test Case.txt");
		string path1("C:\MyOwnData\Test Case.txt");
		string path2("Test Case.in");
		ifstream file(path2,ios::in);
		//ofstream file(path2, ios::out);
		//file.open();
		if (!file) {
			//getline(file, input);
			while (!file.eof()) {
				file >> input;
				cout << input << endl;

			}
			file.close();
		}
		else cout << "Unable to open file." << endl;*/
		/*for (int i = 0; i < threadsNum; i++) {
			isFinish[i]= false;
		}*/

		system("pause");
		return 0;
	
	
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
