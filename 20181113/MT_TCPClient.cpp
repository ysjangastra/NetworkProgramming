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

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// 클라이언트 측 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) return -1;



	// 접속할 서버 주소(127.0.0.1 , 9000) 구조체 생성 및 값 입력
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	
	// 서버에게 TCP 연결
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	/*서버의 IP, 포트번호를 가지고 접속*/
	if (retval == SOCKET_ERROR) return -1; //연결 오류 발생시 종료

	
	// 시간 데이터 수신 및 화면 출력
	while (1)
	{
		retval = recv(sock, buf, BUFSIZE, 0);
		if (retval == 0) break;

		buf[retval] = '\0';
		printf("%s\n", buf);

	}
	
	// 소켓 및 윈속 종료
	closesocket(sock);
	WSACleanup();
	return 0;
}