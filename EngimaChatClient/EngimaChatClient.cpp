// EngimaChatClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <tchar.h>
#include <thread>
#include <mutex>
#include <conio.h>
#include <string>
#include <sstream>
#include "User/User.h"



#define PORT 8080

char buf[4096] = { 0 }; // User input buffer
std::vector<std::string> allMessages;
std::mutex mtx;



void cls(HANDLE hConsole)
{
	COORD coordScreen = { 0, 0 };    // home for the cursor
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	// Get the number of character cells in the current buffer.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	// Fill the entire screen with blanks.
	if (!FillConsoleOutputCharacter(hConsole,        // Handle to console screen buffer
		(TCHAR)' ',      // Character to write to the buffer
		dwConSize,       // Number of cells to write
		coordScreen,     // Coordinates of first cell
		&cCharsWritten)) // Receive number of characters written
	{
		return;
	}

	// Get the current text attribute.
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
	{
		return;
	}

	// Set the buffer's attributes accordingly.
	if (!FillConsoleOutputAttribute(hConsole,         // Handle to console screen buffer
		csbi.wAttributes, // Character attributes to use
		dwConSize,        // Number of cells to set attribute
		coordScreen,      // Coordinates of first cell
		&cCharsWritten))  // Receive number of characters written
	{
		return;
	}

	// Put the cursor at its home coordinates.
	SetConsoleCursorPosition(hConsole, coordScreen);
}





// 0 - disconnect, 1 - connect, >1 - Regular message.
int connectionStatusMsgCol(std::string s) {
	int loc = 0;
	loc = s.find("disconnected");
	if (loc != std::string::npos) {
		return 0;
	}
	loc = s.find("connected");
	if (loc != std::string::npos) {
		return 1;
	}

	return 2;
}

static void clearBuf(char buf[]) {
	for (int i = 0; i < sizeof(buf) / sizeof(buf[0]); i++) {
		buf[i] = 0;
	}
}



void asyncRecieve(SOCKET clientSocket, HANDLE hConsole, WORD sColor) {
	std::vector<char> in;
	bool writeToVec = true;
	int prev = -1;
	int byteCount;
	while (true) {
		char recvBuf[4096];
		byteCount = recv(clientSocket, recvBuf, 4096, 0);
		if (byteCount > 0) {
			cls(hConsole);
			SetConsoleTextAttribute(hConsole, sColor);
			char* msg = (char*)malloc(100);
			strcpy_s(msg, 100, recvBuf);
			if (recvBuf[0] == '/') {
				writeToVec = false;
			}
			if (writeToVec) {
				std::lock_guard<std::mutex> lock(mtx);
				allMessages.push_back(msg);
			}
			
			for (std::string s : allMessages) {
				std::wstringstream test;
				test << "CONNECTION STATUS: " << connectionStatusMsgCol(s) << ", msg: " << s.c_str();
				OutputDebugString(test.str().c_str());
				switch (connectionStatusMsgCol(s)) {
				case 0:
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
					break;
				case 1:
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					break;
				case 2:
					SetConsoleTextAttribute(hConsole, sColor);
					break;
				default:
					SetConsoleTextAttribute(hConsole, sColor);
					break;
				}

				std::cout << s;
			}
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
			std::cout << "EnigmaChat$>" << buf;
			free(msg);
			writeToVec = true;
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


	LPWSTR title = LPWSTR(L"EnigmaChat");
	SetConsoleTitle(title);

	char user[4096] = {0}; // Get In the user thing... (Not all that useful but oh well)
	std::cin.getline(user, 4096);
	
	std::cout << "Entered User Name: " << user << "\n";
	exampleUser.setUserName(user);

	// ??? HOW THE FUCK IS THIS "MORE SECURE" THAN THE WAY IT WORKS ON UNIX?????? -> FUCK MICROSOFT
	wchar_t* userName = new wchar_t;
	size_t wSize = strlen(user);
	mbstowcs_s(&wSize, userName, strlen(user) + 1, user, strlen(user) + 1);

	std::wstring FUICK = userName;
	std::wstring FUCK = L"EnigmaChat - " + FUICK;
	
	SetConsoleTitle(FUCK.c_str());
	for (int i = 0; i < sizeof(user) / sizeof(user[0]); i++) {
		if ((int)user[i] == 0) {
			user[i] = 127; // Our Delimiter. -> no chance a user inputs this on accident.
			i = sizeof(user) / sizeof(user[0]);
		}
	}
	

	//char pass[4096]; // Dont need this yet. Will implement later.
	//std::cin.getline(pass, 4096);
	//exampleUser.setPassword(pass);


	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);


	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
	WORD attributes = consoleInfo.wAttributes;
	WORD bgColor = (attributes & 0x00F0);
	WORD ERROR_COLOR = FOREGROUND_RED | bgColor;
	WORD SUCCESS_COLOR = FOREGROUND_GREEN | FOREGROUND_RED | bgColor;

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
	

	std::string testingshitout = "";
	
	
	bool quit = false;
	bool writeToVec = true;

	COORD loc = { 0, 0 };
	SetConsoleCursorPosition(hConsole, loc);
	cls(hConsole);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
	std::cout << "Loading Messages...";
	cls(hConsole);

	std::thread t1(asyncRecieve, clientSocket, hConsole, SUCCESS_COLOR); // Seperate Thread to read 
	t1.detach();
	int lastMsgListSize = -1;
	
	short curBotLine = 0;
	short curRightCol = 0;
	while (!quit) {
		if (lastMsgListSize != allMessages.size()) {
			cls(hConsole);
			lastMsgListSize = allMessages.size();
			SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
			for (std::string a : allMessages) {
				switch (connectionStatusMsgCol(a)) {
				case 0:
					SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
					break;
				case 1:
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
					break;
				case 2:
					SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
					break;
				default:
					SetConsoleTextAttribute(hConsole, SUCCESS_COLOR);
					break;
				}
				std::cout << a;
			}
			SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN);
			std::cout << "EnigmaChat$>";
		}
		
		

		std::cin.getline(buf, 200);
		char testBuf[4096];
		ZeroMemory(testBuf, 4096);


		if (buf[0] == '/' || buf[0] == 0) {
			writeToVec = false;
		}

		if (buf[0] == '/' && buf[1] == 'q') {
			std::cout << "QUIT SERVER\n";
			
			quit = true;
		}


		


		int byteCount = 0;
		if (writeToVec)
			byteCount = send(clientSocket, buf, 4096, 0);
		else
			byteCount = send(clientSocket, testBuf, 4096, 0);
		if (byteCount > 0) {
			std::string userContainer = user;
			std::string convertedBuf = buf;
			userContainer += ": " + convertedBuf + "\n";
			std::lock_guard<std::mutex> lock(mtx);
			if(writeToVec)
				allMessages.push_back(userContainer);
			clearBuf(std::ref(buf));
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
		writeToVec = true;
	}
	

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	// Close socket and perform final cleanup
	system("pause");
	WSACleanup();
	return 0;

}
