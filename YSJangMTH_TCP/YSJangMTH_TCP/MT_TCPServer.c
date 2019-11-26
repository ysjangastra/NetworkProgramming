#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512 //���� ������ ����
#define SERVERPORT 9000 //���� ��Ʈ ��ȣ 9000������ ����


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

	//���� ����� ��� ���� �ݱ�
	closesocket(listen_sock);

	//������ ���� ����
	WSACleanup();
	return 0;
}