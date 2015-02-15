#include <Windows.h>
#include <process.h>
#include <ctime>
#include <iostream>
using namespace std;

HANDLE semMutex;
HANDLE fillCount;
HANDLE emptyCount;
CRITICAL_SECTION printInfo;
int items = 0;

DWORD WINAPI Producer( LPVOID lpParam )
{
	int lp = (int) lpParam;
	while(true){
		WaitForSingleObject(emptyCount,INFINITE);
		WaitForSingleObject(semMutex,INFINITE);
		//EnterCriticalSection(&printInfo);
		items++;
		cout << "Producer #" << lp << " puts an item. Total: " << items << " items" << endl;
		//LeaveCriticalSection(&printInfo);
		Sleep(rand() % 3000);
		ReleaseSemaphore(semMutex,1,NULL);
		ReleaseSemaphore(fillCount,1,NULL);
	}
}

DWORD WINAPI Consumer( LPVOID lpParam )
{
	int lp = (int) lpParam;
	while(true){
		WaitForSingleObject(fillCount,INFINITE);
		WaitForSingleObject(semMutex,INFINITE);
		//EnterCriticalSection(&printInfo);
		items--;
		cout << "Consumer #" << lp << " took an item. Total: " << items << " items" << endl;
		//LeaveCriticalSection(&printInfo);
		Sleep(rand() % 9000);
		ReleaseSemaphore(semMutex,1,NULL);
		ReleaseSemaphore(emptyCount,1,NULL);
	}
}

int main() {
	srand(time(NULL));
	HANDLE threads[15];
	InitializeCriticalSection(&printInfo);
	semMutex = CreateSemaphore(NULL,1,1,NULL);
	fillCount = CreateSemaphore(NULL,0,10,NULL);
	emptyCount = CreateSemaphore(NULL,10,10,NULL);
	for(int i=0; i < 1; i++) threads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) Producer,(LPVOID) i,0,NULL);
	for( int i=1; i < 15; i++ )
	{
		threads[i] = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE) Consumer,(LPVOID) i,0,NULL);
	}
	WaitForMultipleObjects(15, threads, TRUE, INFINITE);

	for( int i=0; i < 15; i++ )
		CloseHandle(threads[i]);

	CloseHandle(semMutex);
	CloseHandle(fillCount);
	CloseHandle(emptyCount);

	system("pause");
	return 0;
}