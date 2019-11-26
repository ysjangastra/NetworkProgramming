#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1" //Loopback �ּ�
#define SERVERPORT 9000
#define BUFSIZE 512
//���� IP, ��Ʈ ��ȣ, ���� ũ�� ����

void err_quit(char *msg) {
	LPVOID lpMsgBuf; //���� �޼��� ���� ����
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);//���� �޼��� ��¿Ϸ� �� �޸� �Ҵ� ����
	exit(1); //���� ��� �� ����
}

void err_display(char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s]%s", msg, (char *)lpMsgBuf); //CLI �� ǥ��
	LocalFree(lpMsgBuf); //���� �޼��� ��¿Ϸ� �� �޸� �Ҵ� ���� 
}

int recvn(SOCKET s, char *buf, int len, int flags) { 
	//����� ���� ������ ���� �Լ�
	int received;
	char *ptr = buf; //���� ������ ����
	int left = len; //���� ���� (������ ���� ���� ������)

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR) return SOCKET_ERROR;
		/*���� ���� �߻� �� ���� ���� ���� �״�� �Ѱ��ش�*/
		else if (received == 0) break;
		//���ŵ� ���� ���ٸ� ����������
		left -= received; //������ ���� ���̴� ������ ��ŭ ����
		ptr += received; //���� �����ʹ� ������ ��ŭ ����
	}

	return (len - left);//���� ��뷮 ���� ��ȯ
}

int main(int argc, char *argv[]) {
	int retval;

	//������ ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	//���� �ʱ�ȭ
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//IPv4, TCP ��� ���
	if (sock == INVALID_SOCKET) err_quit("socket()");
	 // ���� ������ �޼��� ����� ����

	//�����ϱ�
	SOCKADDR_IN serveraddr; //������ ���� �ּҸ� ������ ����ü ����
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //�ּ� ������ ����ü�� �ʱ�ȭ
	serveraddr.sin_family = AF_INET; // IP ��� ���
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP); // ���� IP�� ����ü�� ����
	serveraddr.sin_port = htons(SERVERPORT);// ���� ��Ʈ ��ȣ ����ü�� ����
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)); 
	/*������ IP, ��Ʈ��ȣ�� ������ ����*/
	if (retval == SOCKET_ERROR) err_quit("Connect()"); //���� ���� �߻��� ����

	char buf[BUFSIZE + 1];//NULL�� ������ ũ��� ���� ����
	int len; //���� ���� ����

	while (1) {
		//������ �Է��ϱ�
		printf("\n[Data for Send] : ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) 
			break; //�Էµ� �����Ͱ� ������ While�� ����������

		len = strlen(buf); //������ ���� ���Ͽ� len ������ �� ����
		if (buf[len - 1] == '\n') buf[len - 1] = '\0'; 
		/*�ٹٲ� ���ڸ� ���۽��� ������ ������ NULL�� �ٲٱ�*/
		if (strlen(buf) == 0) break; 
		/*���۳��� �ƹ� ���뵵 ������ While�� ����������*/

		//������ �����ϱ�
		retval = send(sock, buf, strlen(buf), 0);
		/*��� ���� ����ü��  ���� �ּҷ� �����͸� �۽��ϱ� ����
		  �۽��� ������ ������ ũ���� �۽� ���ۿ� ���� */
		if (retval == SOCKET_ERROR) {
			err_display("send()"); //���� �߻��� �޼��� ���
			break;
		}

		printf("[TCP Client] %d ����Ʈ ���۵�.\n", retval);

		//������ �����ϱ�
		retval = recvn(sock, buf, retval, 0);
		/*��� ���� ����ü�� ���� �ּҷκ��� �����͸� �����Ͽ�
	      ���ŵ� ������ ������ ũ���� ���� ���ۿ� ���� */
		if (retval == SOCKET_ERROR) {
			err_display("recv()");//���� �߻��� �޼��� ���
			break;
		}
		else if (retval == 0)  //���� ������ ������ ����
			break;

		//������ ������ ���
		buf[retval] = '\n'; //������ null�� �ٹٲ� ���ڷ� ����
		printf("[TCP Client] %d ����Ʈ ������.\n", retval);
		printf("[���� ������] %s\n", buf);
		//���� �޼��� ���
	}

	//���� ����� ���´ٸ� �ݱ�
	closesocket(sock);

	//������ ���� ����
	WSACleanup();
	return 0;
}