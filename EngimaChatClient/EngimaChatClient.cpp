// EngimaChatClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <tchar.h>
#include <thread>
#include "User/User.h"



#define PORT 8080


void asyncRecieve(SOCKET clientSocket, HANDLE hConsole, WORD sColor) {
	int byteCount;
	while (true) {
		char recvBuf[4096];
		byteCount = recv(clientSocket, recvBuf, 4096, 0);
		if (byteCount > 0) {
			SetConsoleTextAttribute(hConsole, sColor);
			std::cout << recvBuf;
		}
		else {
			WSACleanup();
		}
	}
	
}



int main(int argc, char* argv[])
{
	std::cout << "Welcome To EnigmaChat, enter a username:";
	User exampleUser;

	char user[4096];
	std::cin.getline(user, 4096);
	exampleUser.setUserName(user);

	char pass[4096];
	std::cin.getline(pass, 4096);
	exampleUser.setPassword(pass);


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
		std::cout << "CONNECTION FAILED\n" << WSAGetLastError() << "\n";
		WSACleanup();
		return 0;
	}
	else {
		SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
		std::cout << "CONNECTION SUCCESS\n";
		std::cout << "CLIENT CAN SEND AND RECV DATA\n";
	}
	// Prior to connection, add our user Data, and things like that ....
	send(clientSocket, user, 4096, 0);



	//  Read a message from the console, and send that message to the server.
	std::thread t1(asyncRecieve, clientSocket, hConsole, SUCCESS_COLOR);
	t1.detach();
	char buf[4096]; // User input buffer
	bool quit = false;
	while (!quit) {
		std::cout << "EnigmaChat$>";
		std::cin.getline(buf, 200);

		if (buf[0] == '/' && buf[1] == 'q') {
			std::cout << "QUIT SERVER\n";
			quit = true;
		}


		int byteCount = send(clientSocket, buf, 4096, 0);
		if (byteCount > 0) {
			SetConsoleTextAttribute(hConsole, bgColor | FOREGROUND_BLUE);
			std::cout << "EnigmaChat$> MESSAGE SENT: " << buf << "\n";
		}
		else {
			WSACleanup();
		}


		// This portion retrieves from the server, when it broadcasts
		/*
		char recvBuf[4096];
		byteCount = recv(clientSocket, recvBuf, 4096, 0);
		if (byteCount > 0) {
			SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
			std::cout << recvBuf;
		}
		else {
			WSACleanup();
		}
		*/
	}
	

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	// Close socket and perform final cleanup
	system("pause");
	WSACleanup();
	return 0;

}
