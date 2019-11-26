#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFSIZE 512

// 윈도우 메시지 처리 함수 원형  
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


// 에디트 컨트롤 출력 함수 원형(실습) **************************************************** 
void DisplayText(char *fmt, )

// ***************************************************************************************

// 소켓 통신 함수 원형 (실습) ************************************************************
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

// ***************************************************************************************

// 오류 출력 함수 원형
void err_quit(char *msg);
void err_display(char *msg);

HINSTANCE hInst; // 인스턴스 핸들
HWND hEdit; // 에디트 컨트롤
CRITICAL_SECTION cs; // 임계영역

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE,
	LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	
	
	// 임계영역 시작 (실습) *****************************************************************
	InitializeCriticalSection(&cs);

	// *************************************************************************************


	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = "MyWindowClass";
	wndclass.lpszMenuName = NULL;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = (WNDPROC)WndProc;
	if (!RegisterClass(&wndclass)) return -1;
	
	// 윈도우 생성 (실습) *****************************************************************
	HWND hWnd = CreateWindow("MyWndClass", "TCPServer", WS_OVERLAPPEDWINDOW, 0, 0, 600, 200, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL) return -1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	
	// ************************************************************************************

	if (hWnd == NULL) return -1;
	
	// 윈도우 show & update 
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// 소켓 통신을 담당하는 스레드 생성 (실습) ********************************************
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
	
	// ************************************************************************************

	// 메시지 루프 
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	// 임계영역 종료(실습) *****************************************************************
	DeleteCriticalSection(&cs);

	// *************************************************************************************
	return msg.wParam;
}

// 윈도우 메시지 처리
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg,
	WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:  
		hEdit = CreateWindow("edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL |
			WS_VSCROLL | ES_AUTOHSCROLL |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);
		return 0;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// socket() 대기소켓 생성 (실습) ******************************************************
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	// ************************************************************************************

	// bind() (실습) **********************************************************************
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9000);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
	// ************************************************************************************

	// listen() (실습) ********************************************************************
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	// ************************************************************************************

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	HANDLE hThread;
	DWORD ThreadId;

	while (1) {
		addrlen = sizeof(clientaddr);
		// accept() (실습) ****************************************************************
		//접속 수락 : accept()
		addrlen = sizeof(clientaddr);//클라이언트 주소길이를 변수에 삽입
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		/* 대기 소켓으로 부터 클라이언트의 IP와 포트 번호를 넘겨 받아서 클라이언트 소켓 생성 */
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()"); //소켓이 제대로 되지 않은 경우 에러 메시지 표시
			break;
		}

		// ********************************************************************************

		DisplayText("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		// 클라이언트 스레드 생성 (실습) **************************************************
		hThread = CreateThread(NULL, 0, ProcessClient, (LPVOID)client_sock, 0, &ThreadId);
		// ********************************************************************************
		if (hThread == NULL)
			DisplayText("[오류] 스레드 생성 실패!\r\n");
		else
			CloseHandle(hThread);
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	SOCKET client_sock = (SOCKET)arg;
	char buf[BUFSIZE + 1];
	SOCKADDR_IN clientaddr;
	int addrlen;
	int retval;

	// 클라이언트 정보 얻기 
	addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);
	

	while (1) {

     // 데이터 받기, 받은 데이터 출력, 데이터 보내기 (실습) ****************************
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




		// 데이터 보내기
		//데이터 전송하기 : send()
		retval = send(client_sock, buf, retval, 0);
		/*클라이언트 통신 소켓 구조체의 주소로 데이터를 송신하기 위해
		송신할 내용을 지정된 크기의 송신 버퍼에 쓴다 */
		if (retval == SOCKET_ERROR) {
			err_display("send()"); //오류 발생시 메세지를 표시한다
			break;
		}



	// ********************************************************************************
	}

	// closesocket()
	closesocket(client_sock);
	DisplayText("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	return 0;
}

// 에디트 컨트롤에 문자열 출력 (실습) *******************************************
void DisplayText(char *fmt, ...)
{
	va_list arg;
	va_start(arg, fnt);

	char cbuf[BUFSIZE + 256];
	vsprinft(cbuf, fmt, arg);

	EnterCriticalSection(&cs);
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	sendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	LeaveCriticalSection(&cs);

	va_end(arg);



}

// ********************************************************************************

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(-1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s", msg, (LPCTSTR)lpMsgBuf);
	LocalFree(lpMsgBuf);
}