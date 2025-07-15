// EngimaChatClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <tchar.h>



#define PORT 8080

int main(int argc, char* argv[])
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	WORD attributes = consoleInfo.wAttributes;
	WORD bgColor = (attributes & 0x00F0);
	WORD ERROR_COLOR = FOREGROUND_RED | bgColor;
	WORD SUCCESS_COLOR = FOREGROUND_GREEN | bgColor;

	SOCKET clientSocket;
	int port = PORT;
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0) {
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "WINSOCK NOT FOUND\n";
		return 0;
	}
	else {
		SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
		std::cout << "WINSOCK FOUND!\n";
		std::cout << "STATUS: " << wsaData.szSystemStatus << "\n";
	}
	clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "ERROR AT SOCKET: " << WSAGetLastError() << "\n";
		return 0;
	}
	else {
		SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
		std::cout << "SOCKET IS GOOD!\n";
	}


	// Connect socket to the server
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(port);
	if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		SetConsoleTextAttribute(hConsole, ERROR_COLOR);
		std::cout << "CONNECTION FAILED\n";
		WSACleanup();
		return 0;
	}
	else {
		SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
		std::cout << "CONNECTION SUCCESS\n";
		std::cout << "CLIENT CAN SEND AND RECV DATA\n";
	}

	//  Read a message from the console, and send that message to the server.

	char buf[200]; // User input buffer
	while (buf != "COMMAND-EXIT") {
		std::cout << "EnigmaChat$>";
		std::cin.getline(buf, 200);

		int byteCount = send(clientSocket, buf, 200, 0);
		if (byteCount > 0) {
			SetConsoleTextAttribute(hConsole, bgColor | FOREGROUND_BLUE);
			std::cout << "EnigmaChat$> MESSAGE SENT: " << buf << "\n";
		}
		else {
			WSACleanup();
		}

		char recvBuf[400];
		byteCount = recv(clientSocket, recvBuf, 400, 0);
		if (byteCount > 0) {
			SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
			std::cout << recvBuf;
		}
		else {
			WSACleanup();
		}

	}
	

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	// Close socket and perform final cleanup
	system("pause");
	WSACleanup();
	return 0;

}
