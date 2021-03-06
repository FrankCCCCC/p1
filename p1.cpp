// p1v2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>
#include <string>

using namespace std;

int m = 0;
int threadsNum;
int checkNum;
int lastRow = 0;
bool allScanFinish = false;
bool allDel = false;
bool allCheckFinish = false;
clock_t* finish;
int* allTimes;
ifstream input_file;
ofstream output_file;

class Coord {
public:
	Coord(int row, int col) : row(row), col(col) {};

	int row;
	int col;
};

class ThreeRow {
public:
	ThreeRow() :prev(NULL), current(NULL), next(NULL) {};
	ThreeRow(vector<int>* prev, vector<int>* current, vector<int>* next) : prev(prev), current(current), next(next) {};
	ThreeRow(vector<int>* prev, vector<int>* current) : prev(prev), current(current), next(NULL) {};
	ThreeRow(vector<int>* current) : prev(NULL), current(current), next(NULL) {};
	vector<int>* prev;//上一行vector<int>的指標
	vector<int>* current;//此行vector<int>的指標
	vector<int>* next;//下一行vector<int>的指標
};

class RowStatus {
public:
	RowStatus(int n, bool scan, bool check, vector<int>* prev, vector<int>* row, RowStatus* next) : n(n), scan(scan), check(check), neighborRow(prev, row), next(next) {};
	RowStatus(int n, vector<int>* prev, vector<int>* row) : n(n), scan(false), check(false), neighborRow(prev, row), next(NULL) {};
	RowStatus(int n, vector<int>* row) : n(n), scan(false), check(false), neighborRow(row), next(NULL) {};
	RowStatus() :n(0), scan(false), check(false), neighborRow(), next(NULL) {};

	RowStatus* newRowStatus(int i, vector<int>* v) {
		RowStatus* newRowSta = new RowStatus{ i, neighborRow.current, v };
		neighborRow.next = v;
		return newRowSta;
	}

	int n;//第幾行
	bool scan;//狀態值
	bool check;//狀態值
	RowStatus* next;//下一行RowStatus的指標

private:
	ThreeRow neighborRow;//該行的讀入數值(vector<int> current)與相鄰兩行(vector<int> next, prev)的指標
	friend void manage(int, int);//宣告manage function為友函數，可以直接純取private變數
};

class CheckResult {
public:
	CheckResult() :forCheckRow(NULL), forCheck(NULL), isThreadFinish(false), scanNum(0) {};

	RowStatus* forCheckRow;//正在判別的行的RowStatus指標
	ThreeRow* forCheck;//正在判別的行的RowStatus->neighborRow指標
	bool isThreadFinish;//線程結束與否
	vector<Coord> peaks;//紀錄峰值的向量
	int scanNum;//紀錄掃描的元素或行數數量
};

RowStatus* tail = NULL;
CheckResult* result = NULL;

void scan(int row, int col) {
	int t;
	RowStatus* tempRow = NULL;
	for (int i = 0; i < row; i++) {
		vector<int>* v = new vector<int>;
		RowStatus* r = NULL;
		for (int j = 0; j < col; j++) {
			input_file >> t;
			v->push_back(t);
		}
		if (tempRow == NULL) {
			r = new RowStatus{ i, v };
			tempRow = r;
			tail = r;
		}
		else {
			r = tempRow->newRowStatus(i, v);
			tempRow->next = r;
			if (r->n == row - 1) {
				r->check = false;
				r->scan = true;
			}
			tempRow->check = false;
			tempRow->scan = true;
			tempRow = r;
		}
		lastRow = i;
	}
	allScanFinish = true;
	//system("exit");
}

void check(int i, CheckResult* resultC, int row, int col) {
	resultC->scanNum = 0;
	while (true) {
		if (resultC->forCheck != NULL && resultC->forCheckRow != NULL) {
			RowStatus* temp = resultC->forCheckRow;
			ThreeRow* tempThreeRow = resultC->forCheck;
			bool slope = tempThreeRow->current->at(1) >= tempThreeRow->current->at(0);
			bool firstSlope = tempThreeRow->current->at(1) > tempThreeRow->current->at(0);;
			resultC->scanNum++;
			if (temp->n == 0) {//If TAIL
				if (!firstSlope) {//First Node Of Row
					if (tempThreeRow->current->at(0) >= tempThreeRow->next->at(0)) {
						Coord c(temp->n + 1, 1);//col-1+1
						resultC->peaks.push_back(c);
					}
				}
				for (int k = 2; k < col; k++) {//Middle Nodes of Row
					resultC->scanNum = resultC->scanNum + 1;
					bool tempSlope = tempThreeRow->current->at(k) >= tempThreeRow->current->at(k - 1);
					if (tempSlope != slope) {
						if (slope && (!tempSlope)) {
							while (tempThreeRow->next == NULL) {}//Next Row is NULL
							if (tempThreeRow->current->at(k - 1) >= tempThreeRow->next->at(k - 1)) {
								Coord c(temp->n + 1, k);//col-1+1
								resultC->peaks.push_back(c);
							}
						}
						slope = tempSlope;
					}
					else if (tempThreeRow->current->at(k) == tempThreeRow->current->at(k - 1) && slope) {
						if (tempThreeRow->current->at(k - 1) >= tempThreeRow->next->at(k - 1)) {
							Coord c(temp->n + 1, k);//col-1+1
							resultC->peaks.push_back(c);
						}
					}
				}
				if (tempThreeRow->current->at(col - 1) >= tempThreeRow->current->at(col - 2)) {//Last Node of Row
					if (tempThreeRow->current->at(col - 1) >= tempThreeRow->next->at(col - 1)) {
						Coord c(temp->n + 1, col);//col-1+1
						resultC->peaks.push_back(c);
					}
				}
				resultC->scanNum++;
			}
			else if (temp->n == row - 1) {//If HEAD
				if (!firstSlope) {//First Node Of Row
					if (tempThreeRow->current->at(0) >= tempThreeRow->prev->at(0)) {
						Coord c(temp->n + 1, 1);//col-1+1
						resultC->peaks.push_back(c);
					}
				}
				resultC->scanNum++;
				for (int k = 2; k < col; k++) {//Middle Nodes of Row
					bool tempSlope = tempThreeRow->current->at(k) >= tempThreeRow->current->at(k - 1);
					if (tempSlope != slope) {
						if (slope && (!tempSlope)) {
							if (tempThreeRow->current->at(k - 1) >= tempThreeRow->prev->at(k - 1)) {
								Coord c(temp->n + 1, k);//col-1+1
								resultC->peaks.push_back(c);
							}
						}
						slope = tempSlope;
					}
					else if (tempThreeRow->current->at(k) == tempThreeRow->current->at(k - 1) && slope) {
						if (tempThreeRow->current->at(k - 1) >= tempThreeRow->prev->at(k - 1)) {
							Coord c(temp->n + 1, k);//col-1+1
							resultC->peaks.push_back(c);
						}
					}
				}
				if (tempThreeRow->current->at(col - 1) >= tempThreeRow->current->at(col - 2)) {//Last Node of Row
					if (tempThreeRow->current->at(col - 1) >= tempThreeRow->prev->at(col - 1)) {
						Coord c(temp->n + 1, col);//col-1+1
						resultC->peaks.push_back(c);
					}
				}
				resultC->scanNum++;
			}
			else {//If Middle
				if (!firstSlope) {//First Node Of Row
					if (tempThreeRow->current->at(0) >= tempThreeRow->prev->at(0) && tempThreeRow->current->at(0) >= tempThreeRow->next->at(0)) {
						Coord c(temp->n + 1, 1);//col-1+1
						resultC->peaks.push_back(c);
					}
				}
				resultC->scanNum++;
				for (int k = 2; k < col; k++) {//Middle Nodes of Row
					bool tempSlope = tempThreeRow->current->at(k) >= tempThreeRow->current->at(k - 1);
					if (tempSlope != slope) {
						if (slope && (!tempSlope)) {
							while (tempThreeRow->next == NULL) {}
							if (tempThreeRow->current->at(k - 1) >= tempThreeRow->prev->at(k - 1) && tempThreeRow->current->at(k - 1) >= tempThreeRow->next->at(k - 1)) {
								Coord c(temp->n + 1, k);//col-1+1
								resultC->peaks.push_back(c);
							}
						}
						slope = tempSlope;
					}
					else if (tempThreeRow->current->at(k) == tempThreeRow->current->at(k - 1) && slope) {
						if (tempThreeRow->current->at(k - 1) >= tempThreeRow->prev->at(k - 1) && tempThreeRow->current->at(k - 1) >= tempThreeRow->next->at(k - 1)) {
							Coord c(temp->n + 1, k);//col-1+1
							resultC->peaks.push_back(c);
						}
					}
				}
				if (tempThreeRow->current->at(col - 1) >= tempThreeRow->current->at(col - 2)) {//Last Node of Row
					if (tempThreeRow->current->at(col - 1) >= tempThreeRow->prev->at(col - 1) && tempThreeRow->current->at(col - 1) >= tempThreeRow->next->at(col - 1)) {
						Coord c(temp->n + 1, col);//col-1+1
						resultC->peaks.push_back(c);
					}
				}
				resultC->scanNum++;
			}
			temp->check = true;
			temp->scan = true;
			temp = NULL;
			tempThreeRow = NULL;
			resultC->forCheck = NULL;
			resultC->forCheckRow = NULL;

		}
		if (allDel == true) { resultC->isThreadFinish = true; return; }
	}
}

void manage(int row, int col) {
	RowStatus* tempD = NULL;
	RowStatus* delRow = NULL;
	RowStatus* tempM = NULL;
	while (tail == NULL) {
	}
	while (tempD != tail) {
		tempD = tail;
		delRow = tail;
		tempM = tail;
	}

	while (true) {
		while (true) {
			if (tempD->next != NULL) {
				if (tempD->check == true && tempD->scan == true && tempD->next->check == true && tempD->next->scan == true) {//Delete
					delRow = tempD;
					tempD = tempD->next;
					tail = tempD;
					delete((delRow->neighborRow).current);
					delete(delRow);
				}
				else {
					break;
				}
			}
			else {
				if (tempD->n == row - 1) {
					delRow = tempD;
					tempD = NULL;
					tail = NULL;
					delete((delRow->neighborRow).current);
					delete(delRow);
					allDel = true;
					return;
				}
				else {
					//cout << "c";
					break;
				}
			}
		}

		for (int i = 0; i < checkNum && tempM != NULL; i++) {//manage
			if ((result[i]).forCheck == NULL && (result[i]).forCheckRow == NULL) {
				if (tempM->check == false && tempM->scan == true) {

					tempM->check = true;
					tempM->scan = false;
					(result[i]).forCheck = &(tempM->neighborRow);
					(result[i]).forCheckRow = tempM;
					if (tempM->next != NULL) {
						tempM = tempM->next;
					}
					else {

						break;
					}
				}
			}
		}
	}
}

int main(int argc, char* argv[]) {
	string p(argv[1]);
	string path = "./" + p + "/matrix.data";
	input_file.open(path);
	path = "";
	path = "./" + p + "/final.peak";
	output_file.open(path);


	threadsNum = thread::hardware_concurrency();
	checkNum = threadsNum - 2;
	result = new CheckResult[checkNum];
	finish = new clock_t[threadsNum];
	allTimes = new int[threadsNum];
	int row, col;

	input_file >> row;
	input_file >> col;

	thread scanThread(scan, row, col);
	scanThread.detach();
	thread manageThread(manage, row, col);
	manageThread.detach();
	for (int i = 0; i < checkNum; i++) {
		//cout << i;
		thread checkThread(check, i, result + i, row, col);
		checkThread.detach();
	}

	bool f = true;
	while (f) {
		f = false;
		for (int i = 0; i < checkNum; i++) {
			if (!result[i].isThreadFinish) { f = true; }
		}
	}
	int peakNum = 0;
	for (int i = 0; i < checkNum; i++) {
		peakNum += result[i].peaks.size();
	}
	output_file << peakNum << endl;
	for (int i = 0; i < checkNum; i++) {
		for (Coord j : result[i].peaks) {
			output_file << j.row << " " << j.col << endl;
		}
	}
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
