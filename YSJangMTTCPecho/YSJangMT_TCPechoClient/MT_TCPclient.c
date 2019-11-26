#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERIP "127.0.0.1" //Loopback 주소
#define SERVERPORT 9000
#define BUFSIZE 512
//서버 IP, 포트 번호, 버퍼 크기 지정

void err_quit(char *msg) {
	LPVOID lpMsgBuf; //오류 메세지 버퍼 정의
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);//오류 메세지 출력완료 후 메모리 할당 해제
	exit(1); //오류 출력 후 종료
}

void err_display(char *msg) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s]%s", msg, (char *)lpMsgBuf); //CLI 상에 표시
	LocalFree(lpMsgBuf); //오류 메세지 출력완료 후 메모리 할당 해제 
}

int recvn(SOCKET s, char *buf, int len, int flags) { 
	//사용자 정의 데이터 수신 함수
	int received;
	char *ptr = buf; //버퍼 포인터 정의
	int left = len; //남은 길이 (최초의 값은 버퍼 사이즈)

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR) return SOCKET_ERROR;
		/*소켓 오류 발생 시 전달 받은 값은 그대로 넘겨준다*/
		else if (received == 0) break;
		//수신된 값이 없다면 빠져나가기
		left -= received; //버퍼의 남은 길이는 수신한 만큼 감소
		ptr += received; //버퍼 포인터는 수신한 만큼 증가
	}

	return (len - left);//버퍼 사용량 값을 반환
}

int main(int argc, char *argv[]) {
	int retval;

	//윈도우 소켓 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;

	//소켓 초기화
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);//IPv4, TCP 방식 사용
	if (sock == INVALID_SOCKET) err_quit("socket()");
	 // 소켓 에러시 메세지 출력후 종료

	//연결하기
	SOCKADDR_IN serveraddr; //접속할 서버 주소를 가지는 구조체 정의
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //주소 저장할 구조체를 초기화
	serveraddr.sin_family = AF_INET; // IP 방식 사용
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP); // 서버 IP를 구조체에 쓰기
	serveraddr.sin_port = htons(SERVERPORT);// 서버 포트 번호 구조체에 쓰기
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)); 
	/*서버의 IP, 포트번호를 가지고 접속*/
	if (retval == SOCKET_ERROR) err_quit("Connect()"); //연결 오류 발생시 종료

	char buf[BUFSIZE + 1];//NULL을 포함한 크기로 버퍼 정의
	int len; //버퍼 길이 변수

	while (1) {
		//데이터 입력하기
		printf("\n[Data for Send] : ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL) 
			break; //입력된 데이터가 없으면 While문 빠져나가기

		len = strlen(buf); //버퍼의 길이 구하여 len 변수에 값 삽입
		if (buf[len - 1] == '\n') buf[len - 1] = '\0'; 
		/*줄바꿈 문자를 전송시의 마지막 문자인 NULL로 바꾸기*/
		if (strlen(buf) == 0) break; 
		/*버퍼내에 아무 내용도 없으면 While문 빠져나가기*/

		//데이터 전송하기
		retval = send(sock, buf, strlen(buf), 0);
		/*통신 소켓 구조체의  서버 주소로 데이터를 송신하기 위해
		  송신할 내용을 지정된 크기의 송신 버퍼에 쓴다 */
		if (retval == SOCKET_ERROR) {
			err_display("send()"); //오류 발생시 메세지 출력
			break;
		}

		printf("[TCP Client] %d 바이트 전송됨.\n", retval);

		//데이터 수신하기
		retval = recvn(sock, buf, retval, 0);
		/*통신 소켓 구조체의 서버 주소로부터 데이터를 수신하여
	      수신된 내용을 지정된 크기의 수신 버퍼에 쓴다 */
		if (retval == SOCKET_ERROR) {
			err_display("recv()");//오류 발생시 메세지 출력
			break;
		}
		else if (retval == 0)  //받은 내용이 없으면 종료
			break;

		//수신한 데이터 출력
		buf[retval] = '\n'; //마지막 null을 줄바꿈 문자로 변경
		printf("[TCP Client] %d 바이트 수신함.\n", retval);
		printf("[수신 데이터] %s\n", buf);
		//수신 메세지 출력
	}

	//소켓 사용을 끝냈다면 닫기
	closesocket(sock);

	//윈도우 소켓 종료
	WSACleanup();
	return 0;
}