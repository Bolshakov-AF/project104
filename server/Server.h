#pragma once

#include <iostream> 
#include <string>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")
using namespace std;

constexpr int messageLen = 1024;

class Server
{
public:
	Server() {}

	~Server() {}

	bool initSock(string ip, uint16_t port) {

		_ip = ip;
		_port = port;

		WSADATA WSAData; 
		WORD ver = MAKEWORD(2, 2);

		int wsInit = WSAStartup(MAKEWORD(2, 2), &WSAData);

		if (wsInit != 0) {
			cout << "Error: can't initialize Winsock." << endl;
			return false;
		}
		return true;
	}

	void StartServer() {

		server = socket(AF_INET, SOCK_STREAM, 0); 

		if (server == INVALID_SOCKET) {
			cout << "Socket creation failed with error:" << WSAGetLastError() << endl;
			return;
		}
		SOCKADDR_IN serverAddr, clientAddr; 
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(_port);

		if (bind(server, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
			cout << "Bind function failed with error: " << WSAGetLastError() << endl;
			return;
		}

		if (listen(server, 0) == SOCKET_ERROR) { 
			cout << "Listen function failed with error:" << WSAGetLastError() << endl;
			return;
		}
		cout << "*Listening for incoming connections." << endl;

		int clientAddrSize = sizeof(clientAddr); 
		if ((client = accept(server, (SOCKADDR*)&clientAddr, &clientAddrSize)) != INVALID_SOCKET) {			
			cout << "Client connected" << endl;
		}
	}

	void closeServer() {
		closesocket(client); 

		if (closesocket(server) == SOCKET_ERROR) { 
			cout << "Close socket failed with error: " << WSAGetLastError() << endl;
			return;
		}
		WSACleanup();
	}

	string readMessage() {
		message.clear();
		if (recv(client, buffer, sizeof(buffer), 0) == SOCKET_ERROR) {			
			cout << "recv function failed with error " << WSAGetLastError() << endl;
			return "";
		}
		message = buffer;
		memset(buffer, 0, sizeof(buffer));
		return message;
	}

	void sendMessage(const string& mes) {
		if (send(client, mes.c_str(), sizeof(mes), 0) == SOCKET_ERROR) {
			cout << "send failed with error " << WSAGetLastError() << endl;
			return;
		}
	}

private:
	string _ip{};
	uint16_t _port{};
	SOCKET server, client; 
	char buffer[messageLen] = { 0 }; 
	string message{};
};
