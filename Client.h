#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream> 
#include <string> 
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")
using namespace std;

constexpr int messageLen = 1024;

class Client
{
public:
	Client() {}

	~Client() {}

	bool initSock(string ip, int port) {

		_ip = ip;
		_port = port;

		WSADATA WSAData;  
		int wsInit = WSAStartup(MAKEWORD(2, 2), &WSAData);

		if (wsInit != 0) {
			cout << "Error: can't initialize Winsock." << endl;
			return false;
		}
		return true;
	}

	void StartClient() {

		client = socket(AF_INET, SOCK_STREAM, 0); 

		if (client == INVALID_SOCKET) {
			cout << "Creating a socket failed with error:" << WSAGetLastError() << endl;
			return;
		}

		SOCKADDR_IN clientAddr; 
		clientAddr.sin_addr.s_addr = inet_addr(_ip.c_str()); 
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(_port); 
		if (connect(client, (SOCKADDR*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
			cout << "Failed connect to the server with error: " << WSAGetLastError() << endl;
			return;
		}

		cout << "Connected to server" << endl;
	}

	void closeClient() {
		closesocket(client); 
		WSACleanup();
	}

	string readMessage() {
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
	int _port{};
	SOCKET client; 
	char buffer[messageLen] = { 0 }; 
	string message{};
};
