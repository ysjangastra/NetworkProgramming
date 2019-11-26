#include <stdio.h>
#include <Windows.h>

DWORD WINAPI MyThread(LPVOID arg) { //스레드에서 수행할 내용
	for (int i = 0; i < 1000; i++) {
		printf("Thread started >> %d\n", i);
	}
	return 0;
}

int main() {
	HANDLE hThread;//스레드 핸들러
	DWORD ThreadId; //스레드 ID

	hThread = CreateThread(NULL, 0, MyThread, NULL, 0, &ThreadId);
	//스레드 핸들러 변수에 스레드 생성 값을 삽입
	while (1) {
		Sleep(2000);
		//표시를 위한 여유시간
		/*
		if (TerminateThread(hThread, 0)) { //스레드 종료 명령어 사용 시
			printf("Thread Terminated\n");
			break;
		}
		*/

		SuspendThread(hThread);// 스레드 중지
		printf("Thread Suspended\n");

		Sleep(1000);

		ResumeThread(hThread); // 스레드 재시작
	}
	return 0;
}