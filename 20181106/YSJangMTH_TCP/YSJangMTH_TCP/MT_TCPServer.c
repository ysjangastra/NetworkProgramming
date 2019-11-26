#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define BUFSIZE 512 //���� ������ ����
#define SERVERPORT 9000 //���� ��Ʈ ��ȣ 9000������ ����
typedef struct tm tm;

DWORD WINAPI ProcessClient(LPVOID arg) {
	SOCKET client_sock = (SOCKET)arg;
	char buf[BUFSIZE + 1];
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;

	time_t t; 
	tm *pt;
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	for (int i = 0; i < 10; i++) {
		time(&t); pt = localtime(&pt);
		//sprintf(buf, "%d.%d.%d %dH %dM %dS",pt -> tm_year+1900, pt->tm)
	}
}

int main(int argc, char *argv[]) {
	int retval;

	//������ ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	//������ ���� API ���� 2.2�� ����Ѵ�

	//���� �ʱ�ȭ
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0); //TCP �������� ���
	if (listen_sock == INVALID_SOCKET) return -1; //���� ������ �޼��� ���

	//���ε�: bind()
	SOCKADDR_IN serveraddr; //���� �ּ� ������ �޸� ����
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //������ ������ ���� �޸� �ʱ�ȭ
	serveraddr.sin_family = AF_INET; // IP ��� ���
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*���� IP�� ���� ���� ��밡���� IP �� �ϳ��� ����*/
	serveraddr.sin_port = htons(SERVERPORT); //���� ��Ʈ ��ȣ�� 9000��
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	/*���ε��� �����ϸ� ����ü�� IP�� ��Ʈ��ȣ�� ���� ��� ���� ������*/
	if (retval == SOCKET_ERROR) return -1;
	/*���ε� ���н� ���� �޼��� ��� �� ����*/

	//Ŭ���̾�Ʈ ���� ��� : listen()
	retval = listen(listen_sock, SOMAXCONN);//�ִ� ���� ���� �����ϰ� ���� ���
	if (retval == SOCKET_ERROR) return -1;//���� �߻��� ����������

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;
	DWORD ThreadId;

	while (1) {
		//accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET) {
			printf("accept error");
			continue;
		}
		//������ Ŭ���̾�Ʈ�� IP�� Port ��ȣ�� ����ü�κ��� ������ ǥ��
		printf("\n[TCP Server] Client Access : IP > %s, Port > %d\n"
			, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//������ ����
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, &ThreadId);
		if (hThread == NULL) {
			printf("ERROR : Thread Creation Failed\n");
		}
		else CloseHandle(hThread);
	}

	//���� ����� ��� ���� �ݱ�
	closesocket(listen_sock);

	//������ ���� ����
	WSACleanup();
	return 0;
}