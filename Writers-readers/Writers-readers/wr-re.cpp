#include <Windows.h>
#include <process.h>
#include <iostream>
using namespace std;

CRITICAL_SECTION wrt;
CRITICAL_SECTION rd;
CRITICAL_SECTION printInfo;
int readCounter = 0;

DWORD WINAPI Writer( LPVOID param ) {
	for(int i = 0; i < 30; i++) {
		EnterCriticalSection(&wrt);
		EnterCriticalSection(&printInfo);
		cout << "Writer writes" << endl;
		LeaveCriticalSection(&printInfo);
		LeaveCriticalSection(&wrt);
	}
	return 1;
}

DWORD WINAPI Reader( LPVOID param ) {
	EnterCriticalSection(&rd);
	readCounter++;
	if(readCounter==1) EnterCriticalSection(&wrt);
	LeaveCriticalSection(&rd);
	EnterCriticalSection(&printInfo);
	cout << "Number of readers: " << readCounter << endl;
	LeaveCriticalSection(&printInfo);
	LeaveCriticalSection(&rd);
	readCounter--;
	if (readCounter==0) LeaveCriticalSection(&wrt);
	LeaveCriticalSection(&rd);
	return 1;
}

int main() {
	InitializeCriticalSection(&wrt);
	InitializeCriticalSection(&rd);
	InitializeCriticalSection(&printInfo);
	HANDLE  hThreadArray[10];
	hThreadArray[0] = CreateThread(NULL,0,Writer, (LPVOID) 1,0,0);
	for(int i = 1; i < 10; i++) hThreadArray[i] = CreateThread(NULL,0,Reader, (LPVOID) i,0,0);
	WaitForMultipleObjects(10, hThreadArray, TRUE, INFINITE);
	for(int i=0; i<10; i++) CloseHandle(hThreadArray[i]);
	system("pause");
	return 0;
}