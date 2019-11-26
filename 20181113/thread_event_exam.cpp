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
		// �б� �ϷḦ ��ٸ�
		retval = WaitForSingleObject(hReadEvent, 1000);
		if (retval == WAIT_TIMEOUT) continue;
		if (retval == WAIT_FAILED) break;

		// ���� ���ۿ� ������ ����
		for (int i = 0; i < BUFSIZE; i++) buf[i] = k;

		// ���� �ϷḦ �˸�
		SetEvent(hWriteEvent);//WriteEvent flag�� True�� �ٲ� : read ������ ���
	}

	// �̺�Ʈ ����
	CloseHandle(hWriteEvent);

	return 0;
}

DWORD WINAPI ReadThread(LPVOID arg)
{
	DWORD retval;

	while (1) {
		// ���� �ϷḦ ��ٸ�
		retval = WaitForSingleObject(hWriteEvent, 1000);
		if (retval == WAIT_TIMEOUT) continue;
		if (retval == WAIT_FAILED) break;

		// ���� �����͸� ���
		printf("Thread %d:\t", GetCurrentThreadId());
		for (int i = 0; i < BUFSIZE; i++) {
			printf("%d", buf[i]);
		}
			printf("\n");

		// ���۸� 0���� �ʱ�ȭ
		ZeroMemory(buf, sizeof(buf));

		// �б� �ϷḦ �˸�
		SetEvent(hReadEvent);
	}

	return 0;
}

int main()
{
	// �̺�Ʈ ����, hReadEvent, hWriteEvent
	hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (hReadEvent == NULL) return - 1;
	hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (hWriteEvent == NULL) return -1;

	// �� ���� ������ ����
	HANDLE hThread[3];
	DWORD ThreadId[3];
	hThread[0] = CreateThread(NULL, 0, WriteThread, NULL, 0, &ThreadId[0]);
	hThread[1] = CreateThread(NULL, 0, ReadThread, NULL, 0, &ThreadId[1]);
	hThread[2] = CreateThread(NULL, 0, ReadThread, NULL, 0, &ThreadId[2]);

	// ������ ���� ��� (���Ѵ�ð� ����)
	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	// �̺�Ʈ ����
	CloseHandle(hReadEvent);

	printf("��� �۾��� �Ϸ��߽��ϴ�.\n");

	return 0;
}