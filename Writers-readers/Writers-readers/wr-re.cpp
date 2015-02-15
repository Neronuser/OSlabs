#include <Windows.h>
#include <process.h>
#include <ctime>
#include <iostream>
using namespace std;

CRITICAL_SECTION wrt;
CRITICAL_SECTION rd;
CRITICAL_SECTION printInfo;
int readCounter = 0;
bool flag = 1;

DWORD WINAPI Writer( LPVOID param ) {
	while(true) {
		flag=1;
		EnterCriticalSection(&wrt);
		flag=0;
		EnterCriticalSection(&printInfo);
		cout << "Writer writes" << endl;
		Sleep(rand() % 3000);
		LeaveCriticalSection(&printInfo);
		LeaveCriticalSection(&wrt);
		EnterCriticalSection(&printInfo);
		cout << "Writer sleeps" << endl;
		Sleep(rand() % 9000);
		LeaveCriticalSection(&printInfo);
	}
	return 1;
}

DWORD WINAPI Reader( LPVOID param ) {
	while(true) {
		if(!flag) {
			EnterCriticalSection(&rd);
			if(readCounter==0) EnterCriticalSection(&wrt);
			readCounter++;
			LeaveCriticalSection(&rd);
			EnterCriticalSection(&printInfo);
			cout << "Number of readers: " << readCounter << endl;
			Sleep(rand() % 3000);
			LeaveCriticalSection(&printInfo);
			EnterCriticalSection(&rd);
			readCounter--;
			if (readCounter==0) LeaveCriticalSection(&wrt);
			LeaveCriticalSection(&rd);
			}
		//else Sleep(rand() % 3000);
	}
	return 1;
}

int main() {
	srand(time(NULL));
	InitializeCriticalSection(&wrt);
	InitializeCriticalSection(&rd);
	InitializeCriticalSection(&printInfo);
	HANDLE  hThreadArray[4];
	hThreadArray[0] = CreateThread(NULL,0,Writer, (LPVOID) 1,0,0);
	for(int i = 1; i < 4; i++) hThreadArray[i] = CreateThread(NULL,0,Reader, (LPVOID) i,0,0);
	WaitForMultipleObjects(4, hThreadArray, TRUE, INFINITE);
	for(int i=0; i<4; i++) CloseHandle(hThreadArray[i]);
	DeleteCriticalSection(&wrt);
	DeleteCriticalSection(&rd);
	DeleteCriticalSection(&printInfo);
	system("pause");
	return 0;
}