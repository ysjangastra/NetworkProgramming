#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512 //버퍼 사이즈 지정
#define SERVERPORT 9000 //서버 포트 번호 9000번으로 지정

void err_quit(char *msg) { //에러 메시지 출력 후 종료
	LPVOID lpMsgBuf;
	//어떠한 타입이든 받을 수 있는 메세지 버퍼 포인터 정의
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	//위의 구문으로 정의된 버퍼 포인터에 수행 중 마지막으로 발생한 오류를 삽입한다 
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR); //대화 상자로 메세지 출력
	LocalFree(lpMsgBuf); // 메세지 출력 후 자원할당 해제
	exit(1); // 종료
}

void err_display(char *msg) { //에러 메세지 출력 후 프로그램 계속
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s]%s", msg, (char *)lpMsgBuf); //CLI 상에 출력
	LocalFree(lpMsgBuf); //자원할당 해제
}

DWORD WINAPI EchoThread(LPVOID arg) {
	//데이터 통신에 필요한 변수 지정
	SOCKET client_sock = (SOCKET)arg; 
	/*대기 소켓에서 넘겨준 클라이언트 소켓 구조체를 arg가 가지고 있으며, 
	이를 client_sock 소켓 구조체에 넘겨준다*/
	SOCKADDR_IN clientaddr; //클라이언트로부터 받은 IP와 포트번호 저장하는 구조체 변수
	int addrlen;//클라이언트 주소 길이
	char buf[BUFSIZE + 1]; //NULL 문자를 포함하여 크기 지정
	int retval;

	addrlen = sizeof(clientaddr);//클라이언트 주소길이를 변수에 삽입
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);
	//클라이언트 주소 구조체 내부의 정보를 가지고 온다

	while (1) {
		//데이터 수신하기 : recv()
		retval = recv(client_sock, buf, BUFSIZE, 0);
		/*클라이언트 통신 소켓 구조체의 주소로부터 데이터를 수신하여
		수신된 내용을 지정된 크기의 수신 버퍼에 쓴다 */
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break; //소켓 에러 발생시 빠져나가기
		}
		else if (retval == 0) {
			break; // 수신된 내용이 없으면 빠져나가기
		}

		//수신 데이터 출력
		buf[retval] = '\0'; //NULL문자가 수신 데이터의 끝을 나타냄
		printf("[TCP/%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
		//클라이언트의 IP, 포트번호, 수신 받은 데이터(버퍼에 수신된)의 내용을 출력

		//데이터 전송하기 : send()
		retval = send(client_sock, buf, retval, 0);
		/*클라이언트 통신 소켓 구조체의 주소로 데이터를 송신하기 위해
		송신할 내용을 지정된 크기의 송신 버퍼에 쓴다 */
		if (retval == SOCKET_ERROR) {
			err_display("send()"); //오류 발생시 메세지를 표시한다
			break;
		}
	}

	//클라이언트와의 접속 종료 후 통신 소켓 닫기
	closesocket(client_sock);
	printf("[Message] client shutdown: %s:%d\n"
		, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
	/* 클라이언트가 종료되면 종료된 클라이언트의 IP와 포트번호 출력 */

	return 0;
}

int main(int argc, char *argv[]) {
	int retval;
	
	//윈도우 소켓 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1; 
	//윈도우 소켓 API 버전 2.2를 사용한다

	//소켓 초기화
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0); //TCP 프로토콜 사용
	if (listen_sock == INVALID_SOCKET) err_quit("socket()"); //소켓 오류시 메세지 출력

	//바인딩: bind()
	SOCKADDR_IN serveraddr; //서버 주소 지정할 메모리 공간
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //지정한 공간에 대해 메모리 초기화
	serveraddr.sin_family = AF_INET; // IP 방식 사용
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); 
	/*서버 IP는 서버 내에 사용가능한 IP 중 하나를 지정*/
	serveraddr.sin_port = htons(SERVERPORT); //서버 포트 번호는 9000번
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	/*바인딩에 성공하면 구조체의 IP와 포트번호를 토대로 대기 소켓 생성됨*/
	if (retval == SOCKET_ERROR) err_quit("bind()");
	/*바인딩 실패시 오류 메세지 출력 후 종료*/

	//클라이언트 접속 대기 : listen()
	retval = listen(listen_sock, SOMAXCONN);//최대 연결 수를 설정하고 접속 대기
	if (retval == SOCKET_ERROR) err_quit("listen()");//오류 발생시 빠져나가기

	//데이터 통신에 필요한 변수 지정
	SOCKET client_sock; //대기 소켓에서 넘겨 받아서 통신을 수행할 클라이언트 통신 소켓 정의
	SOCKADDR_IN clientaddr; //클라이언트로부터 받은 IP와 포트번호 저장하는 구조체 변수
	int addrlen;//클라이언트 주소 길이

	HANDLE hThread; //스레드 핸들러
	DWORD ThreadId; //스레드 ID

	while (1) {
		//접속 수락 : accept()
		addrlen = sizeof(clientaddr);//클라이언트 주소길이를 변수에 삽입
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		/* 대기 소켓으로 부터 클라이언트의 IP와 포트 번호를 넘겨 받아서 클라이언트 소켓 생성 */
		if (client_sock == INVALID_SOCKET) { 
			err_display("accept()"); //소켓이 제대로 되지 않은 경우 에러 메시지 표시
			break;
		}

		//접속한 클라이언트의 IP와 Port 번호를 구조체로부터 가져와 표시
		printf("\n[TCP Server] Client Access : IP > %s, Port > %d\n"
			, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//클라이언트의 다중 접속 허용을 위해 스레드를 만들어 클라이언트 소켓을 넘겨준다
		hThread = CreateThread(NULL, 0, EchoThread, (LPVOID)client_sock, 0, &ThreadId);
		if (hThread == NULL) {
			printf("[ERROR] Thread creation fail\n");
		} //스레드가 제대로 만들어지지 않았을 경우 오류 표시
		else {
			CloseHandle(hThread); //스레드의 작업이 완료되면 스레드 종료
		}
	}

	//서버 종료시 대기 소켓 닫기
	closesocket(listen_sock);

	//윈도우 소켓 종료
	WSACleanup();
	return 0;
}