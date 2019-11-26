#include <winsock2.h>

int main(int argc, char *argv[]) {
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;
	MessageBox(NULL, "Winsock Initialize Succeed", "Succeed", MB_OK);
	/*
	SOCKET tcp_sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP); //use TCP
	if (tcp_sock == INVALID_SOCKET){
		MessageBox(NULL, "Socket Initialize Failed", "Failed", MB_OK);
		return -1;
	}
	MessageBox(NULL, "Socket Initialize Succeed", "Succeed", MB_OK);
	*/
	SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); //use UDP
	if (udp_sock == INVALID_SOCKET){ 
		MessageBox(NULL, "Socket Initialize Failed", "Failed", MB_OK);
		return -1;
	}
	MessageBox(NULL, "Socket Initialize Succeed", "Succeed", MB_OK);

	//closesocket(tcp_sock); //close socket
	closesocket(udp_sock);
	WSACleanup; //winsock close

	return 0;
}