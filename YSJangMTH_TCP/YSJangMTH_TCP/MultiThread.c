#include <stdio.h>
#include <Windows.h>

DWORD WINAPI MyThread(LPVOID arg) { //�����忡�� ������ ����
	for (int i = 0; i < 1000; i++) {
		printf("Thread started >> %d\n", i);
	}
	return 0;
}

int main() {
	HANDLE hThread;//������ �ڵ鷯
	DWORD ThreadId; //������ ID

	hThread = CreateThread(NULL, 0, MyThread, NULL, 0, &ThreadId);
	//������ �ڵ鷯 ������ ������ ���� ���� ����
	while (1) {
		Sleep(2000);
		//ǥ�ø� ���� �����ð�
		/*
		if (TerminateThread(hThread, 0)) { //������ ���� ��ɾ� ��� ��
			printf("Thread Terminated\n");
			break;
		}
		*/

		SuspendThread(hThread);// ������ ����
		printf("Thread Suspended\n");

		Sleep(1000);

		ResumeThread(hThread); // ������ �����
	}
	return 0;
}