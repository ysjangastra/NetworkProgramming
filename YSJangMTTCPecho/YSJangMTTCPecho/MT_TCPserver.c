#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512 //���� ������ ����
#define SERVERPORT 9000 //���� ��Ʈ ��ȣ 9000������ ����

void err_quit(char *msg) { //���� �޽��� ��� �� ����
	LPVOID lpMsgBuf;
	//��� Ÿ���̵� ���� �� �ִ� �޼��� ���� ������ ����
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	//���� �������� ���ǵ� ���� �����Ϳ� ���� �� ���������� �߻��� ������ �����Ѵ� 
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR); //��ȭ ���ڷ� �޼��� ���
	LocalFree(lpMsgBuf); // �޼��� ��� �� �ڿ��Ҵ� ����
	exit(1); // ����
}

void err_display(char *msg) { //���� �޼��� ��� �� ���α׷� ���
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s]%s", msg, (char *)lpMsgBuf); //CLI �� ���
	LocalFree(lpMsgBuf); //�ڿ��Ҵ� ����
}

DWORD WINAPI EchoThread(LPVOID arg) {
	//������ ��ſ� �ʿ��� ���� ����
	SOCKET client_sock = (SOCKET)arg; 
	/*��� ���Ͽ��� �Ѱ��� Ŭ���̾�Ʈ ���� ����ü�� arg�� ������ ������, 
	�̸� client_sock ���� ����ü�� �Ѱ��ش�*/
	SOCKADDR_IN clientaddr; //Ŭ���̾�Ʈ�κ��� ���� IP�� ��Ʈ��ȣ �����ϴ� ����ü ����
	int addrlen;//Ŭ���̾�Ʈ �ּ� ����
	char buf[BUFSIZE + 1]; //NULL ���ڸ� �����Ͽ� ũ�� ����
	int retval;

	addrlen = sizeof(clientaddr);//Ŭ���̾�Ʈ �ּұ��̸� ������ ����
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);
	//Ŭ���̾�Ʈ �ּ� ����ü ������ ������ ������ �´�

	while (1) {
		//������ �����ϱ� : recv()
		retval = recv(client_sock, buf, BUFSIZE, 0);
		/*Ŭ���̾�Ʈ ��� ���� ����ü�� �ּҷκ��� �����͸� �����Ͽ�
		���ŵ� ������ ������ ũ���� ���� ���ۿ� ���� */
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break; //���� ���� �߻��� ����������
		}
		else if (retval == 0) {
			break; // ���ŵ� ������ ������ ����������
		}

		//���� ������ ���
		buf[retval] = '\0'; //NULL���ڰ� ���� �������� ���� ��Ÿ��
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		//Ŭ���̾�Ʈ�� IP, ��Ʈ��ȣ, ���� ���� ������(���ۿ� ���ŵ�)�� ������ ���

		//������ �����ϱ� : send()
		retval = send(client_sock, buf, retval, 0);
		/*Ŭ���̾�Ʈ ��� ���� ����ü�� �ּҷ� �����͸� �۽��ϱ� ����
		�۽��� ������ ������ ũ���� �۽� ���ۿ� ���� */
		if (retval == SOCKET_ERROR) {
			err_display("send()"); //���� �߻��� �޼����� ǥ���Ѵ�
			break;
		}
	}

	//Ŭ���̾�Ʈ���� ���� ���� �� ��� ���� �ݱ�
	closesocket(client_sock);
	printf("[Message] client shutdown: %s:%d\n"
		, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	/* Ŭ���̾�Ʈ�� ����Ǹ� ����� Ŭ���̾�Ʈ�� IP�� ��Ʈ��ȣ ��� */

	return 0;
}

int main(int argc, char *argv[]) {
	int retval;
	
	//������ ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1; 
	//������ ���� API ���� 2.2�� ����Ѵ�

	//���� �ʱ�ȭ
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0); //TCP �������� ���
	if (listen_sock == INVALID_SOCKET) err_quit("socket()"); //���� ������ �޼��� ���

	//���ε�: bind()
	SOCKADDR_IN serveraddr; //���� �ּ� ������ �޸� ����
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //������ ������ ���� �޸� �ʱ�ȭ
	serveraddr.sin_family = AF_INET; // IP ��� ���
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	/*���� IP�� ���� ���� ��밡���� IP �� �ϳ��� ����*/
	serveraddr.sin_port = htons(SERVERPORT); //���� ��Ʈ ��ȣ�� 9000��
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	/*���ε��� �����ϸ� ����ü�� IP�� ��Ʈ��ȣ�� ���� ��� ���� ������*/
	if (retval == SOCKET_ERROR) err_quit("bind()");
	/*���ε� ���н� ���� �޼��� ��� �� ����*/

	//Ŭ���̾�Ʈ ���� ��� : listen()
	retval = listen(listen_sock, SOMAXCONN);//�ִ� ���� ���� �����ϰ� ���� ���
	if (retval == SOCKET_ERROR) err_quit("listen()");//���� �߻��� ����������

	//������ ��ſ� �ʿ��� ���� ����
	SOCKET client_sock; //��� ���Ͽ��� �Ѱ� �޾Ƽ� ����� ������ Ŭ���̾�Ʈ ��� ���� ����
	SOCKADDR_IN clientaddr; //Ŭ���̾�Ʈ�κ��� ���� IP�� ��Ʈ��ȣ �����ϴ� ����ü ����
	int addrlen;//Ŭ���̾�Ʈ �ּ� ����

	HANDLE hThread; //������ �ڵ鷯
	DWORD ThreadId; //������ ID

	while (1) {
		//���� ���� : accept()
		addrlen = sizeof(clientaddr);//Ŭ���̾�Ʈ �ּұ��̸� ������ ����
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		/* ��� �������� ���� Ŭ���̾�Ʈ�� IP�� ��Ʈ ��ȣ�� �Ѱ� �޾Ƽ� Ŭ���̾�Ʈ ���� ���� */
		if (client_sock == INVALID_SOCKET) { 
			err_display("accept()"); //������ ����� ���� ���� ��� ���� �޽��� ǥ��
			break;
		}

		//������ Ŭ���̾�Ʈ�� IP�� Port ��ȣ�� ����ü�κ��� ������ ǥ��
		printf("\n[TCP Server] Client Access : IP > %s, Port > %d\n"
			, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//Ŭ���̾�Ʈ�� ���� ���� ����� ���� �����带 ����� Ŭ���̾�Ʈ ������ �Ѱ��ش�
		hThread = CreateThread(NULL, 0, EchoThread, (LPVOID)client_sock, 0, &ThreadId);
		if (hThread == NULL) {
			printf("[ERROR] Thread creation fail\n");
		} //�����尡 ����� ��������� �ʾ��� ��� ���� ǥ��
		else {
			CloseHandle(hThread); //�������� �۾��� �Ϸ�Ǹ� ������ ����
		}
	}

	//���� ����� ��� ���� �ݱ�
	closesocket(listen_sock);

	//������ ���� ����
	WSACleanup();
	return 0;
}