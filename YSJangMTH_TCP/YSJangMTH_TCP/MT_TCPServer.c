#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512 //버퍼 사이즈 지정
#define SERVERPORT 9000 //서버 포트 번호 9000번으로 지정


int main(int argc, char *argv[]) {
	int retval;

	//윈도우 소켓 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	//윈도우 소켓 API 버전 2.2를 사용한다

	//소켓 초기화
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0); //TCP 프로토콜 사용
	if (listen_sock == INVALID_SOCKET) return -1; //소켓 오류시 메세지 출력

	//바인딩: bind()
	SOCKADDR_IN serveraddr; //서버 주소 지정할 메모리 공간
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //지정한 공간에 대해 메모리 초기화
	serveraddr.sin_family = AF_INET; // IP 방식 사용
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	/*서버 IP는 서버 내에 사용가능한 IP 중 하나를 지정*/
	serveraddr.sin_port = htons(SERVERPORT); //서버 포트 번호는 9000번
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	/*바인딩에 성공하면 구조체의 IP와 포트번호를 토대로 대기 소켓 생성됨*/
	if (retval == SOCKET_ERROR) return -1;
	/*바인딩 실패시 오류 메세지 출력 후 종료*/

	//서버 종료시 대기 소켓 닫기
	closesocket(listen_sock);

	//윈도우 소켓 종료
	WSACleanup();
	return 0;
}