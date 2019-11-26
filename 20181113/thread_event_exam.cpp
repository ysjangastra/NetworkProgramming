#include <windows.h>
#include <stdio.h>

#define BUFSIZE 16

HANDLE hReadEvent;
HANDLE hWriteEvent;
char buf[BUFSIZE];

DWORD WINAPI WriteThread(LPVOID arg)
{
	DWORD retval;

	for (int k = 0; k<10; k++) {
		// 읽기 완료를 기다림
		retval = WaitForSingleObject(hReadEvent, 1000);
		if (retval == WAIT_TIMEOUT) continue;
		if (retval == WAIT_FAILED) break;

		// 공유 버퍼에 데이터 쓰기
		for (int i = 0; i < BUFSIZE; i++) buf[i] = k;

		// 쓰기 완료를 알림
		SetEvent(hWriteEvent);//WriteEvent flag를 True로 바꿈 : read 스레드 깨어남
	}

	// 이벤트 제거
	CloseHandle(hWriteEvent);

	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg)
{
	DWORD retval;

	while (1) {
		// 쓰기 완료를 기다림
		retval = WaitForSingleObject(hWriteEvent, 1000);
		if (retval == WAIT_TIMEOUT) continue;
		if (retval == WAIT_FAILED) break;

		// 읽은 데이터를 출력
		printf("Thread %d:\t", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++) {
			printf("%d", buf[i]);
		}
			printf("\n");

		// 버퍼를 0으로 초기화
		ZeroMemory(buf, sizeof(buf));

		// 읽기 완료를 알림
		SetEvent(hReadEvent);
	}

	return 0;
}

int main()
{
	// 이벤트 생성, hReadEvent, hWriteEvent
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return - 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return -1;

	// 세 개의 스레드 생성
	HANDLE hThread[3];
	DWORD ThreadId[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, &ThreadId[0]);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, &ThreadId[1]);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, &ThreadId[2]);

	// 스레드 종료 대기 (무한대시간 동안)
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	// 이벤트 제거
	CloseHandle(hReadEvent);

	printf("모든 작업을 완료했습니다.\n");

	return 0;
}