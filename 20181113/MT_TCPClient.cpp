#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1"
#define SERVERPORT 9000
#define BUFSIZE 4096

int main(int argc, char*argv[]) {
	int retval;
	char buf[BUFSIZE + 1];

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// Ŭ���̾�Ʈ �� ���� ����
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) return -1;



	// ������ ���� �ּ�(127.0.0.1 , 9000) ����ü ���� �� �� �Է�
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	
	// �������� TCP ����
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	/*������ IP, ��Ʈ��ȣ�� ������ ����*/
	if (retval == SOCKET_ERROR) return -1; //���� ���� �߻��� ����

	
	// �ð� ������ ���� �� ȭ�� ���
	while (1)
	{
		retval = recv(sock, buf, BUFSIZE, 0);
		if (retval == 0) break;

		buf[retval] = '\0';
		printf("%s\n", buf);

	}
	
	// ���� �� ���� ����
	closesocket(sock);
	WSACleanup();
	return 0;
}