#pragma once

#include "Client.h"
#include <string>
#include <iostream>
using namespace std;

class Chat final {
public:
	Chat(const string& ip, const uint16_t port) :
		_ip(ip), _port(port), _connection(false), _isUserOnline(false) {

		if (_client.initSock(_ip, _port)) _client.StartClient();
	}

	void Start() {
		_connection = true;		
		string message{};
		cout << "\n\t*Welcome to Chat*" << endl;
		while (_connection)
		{			
			if (!_isUserOnline) {
				cout << " \n1 - Autorization \n2 - Registration \n0 - Exit" << endl;
			}
			else {
				cout << "\n*You have entered the User menu*" << endl;
				cout << "\n3 - Show all users \n4 - Show all messages"
					<< "\n5 - Add message to chat \n6 - Change password \n7 - Sign out" << endl;
			}

			int operation{};
			cout << "User: " << endl;			
			cin >> operation;
			_client.sendMessage(to_string(operation));
			message = _client.readMessage();
			cout << "Server: " << message << endl;
			if (message[0] == '1') {
				string name, login, password;
				int usersCount{};
				bool loginS = false;
				bool passwordS = false;
				string to, text;
				int messagesCount{};
				string newPassword{};

				switch (operation)
				{
				case 1:
					cout << "\n\t*Login*" << endl;
					loginS = false;
					passwordS = false;
					while (!loginS) {
						cout << "Enter your login: " << endl;
						cin >> login;
						_client.sendMessage(login);
						if (_client.readMessage() == "1") {
							loginS = true;
							while (!passwordS) {
								cout << "Enter your password: " << endl;
								cin >> password;
								_client.sendMessage(password);
								if (_client.readMessage() == "1") {
									cout << "\n*Hello " << login << "!*" << endl;
									_currentUser = login;
									_isUserOnline = true;
									passwordS = true;
								}
								else
									cout << "The password entered is incorrect, please try again" << endl;
							}
						}
						else
							cout << "The entered login is incorrect, please try again" << endl;
					}
					break;
				case 2:
					cout << "\n\t*Signup*" << endl;
					cout << "Enter your name: " << endl;
					cin >> name;
					_client.sendMessage(name);
					while (!passwordS) {
						cout << "Enter your login: " << endl;
						cin >> login;
						_client.sendMessage(login);
						if (_client.readMessage() == "0") {
							cout << "Enter your password: " << endl;
							cin >> password;
							_client.sendMessage(password);
							passwordS = true;
						}
						else
							cout << "Login is invalid, try again" << endl;
					}
					cout << "Server: " << _client.readMessage() << endl;
					_isUserOnline = true;
					_currentUser = login;
					break;
				case 3:
					cout << "\n\t*Show all users in the Chat*" << endl;
					usersCount = stoi(_client.readMessage());
					_client.sendMessage(" ");
					cout << "Count of users: " << usersCount << endl;
					cout << "Logins:\n";
					for (int i = 0; i < usersCount; i++) {
						login = _client.readMessage();
						_client.sendMessage(" ");
						if (login == _currentUser)
							cout << login << " (You)" << endl;
						else
							cout << login << endl;
					}
					break;
				case 4:
					cout << "\n\t*Show all Chat messages*" << endl;
					cout << " \n1 - Show messages for all users \n2 - Show private messages" << endl;
					cout << _currentUser << ": " << endl;
					cin.ignore();
					cin >> operation;
					_client.sendMessage(to_string(operation));
					switch (operation)
					{
					case 1:
						messagesCount = stoi(_client.readMessage());
						if (messagesCount == 0)
							cout << "It's no messages to all in this chat" << endl;
						else {
							cout << "Messages to all: " << messagesCount << endl;
							_client.sendMessage(" ");
							for (int i = 0; i < messagesCount; i++) {
								cout << _client.readMessage() << endl;
								_client.sendMessage(" ");
							}
						}
						break;
					case 2:
						messagesCount = stoi(_client.readMessage());
						if (messagesCount == 0)
							cout << "This message is not for you, " << _currentUser << endl;
						else {
							cout << "Messages to You: " << messagesCount << endl;
							_client.sendMessage(" ");
							for (int i = 0; i < messagesCount; i++) {
								cout << _client.readMessage() << endl;
								_client.sendMessage(" ");
							}
						}
						break;
					default:
						break;
					}
					break;
				case 5:
					cout << "\n\t*Add message to Chat*" << endl;
					loginS = false;
					while (!loginS) {
						cout << "To (Login of user or to \"all\" users): " << endl;
						cin >> to;
						_client.sendMessage(to);
						if (!(to == "all")) {
							if (_client.readMessage() == "1") {
								loginS = true;
								cout << "Enter the text:" << endl;
								cin.ignore();
								getline(cin, text);
								_client.sendMessage(text);
							}
							else
								cout << "Unable to find a user named " << to << endl;
						}
						else {
							loginS = true;
							cout << "Enter the text:" << endl;
							cin.ignore();
							getline(cin, text);
							_client.sendMessage(text);
						}
					}
					break;
				case 6:
					cout << "\n\t*Change password*" << endl;
					cout << "Enter new password:" << endl;
					cin >> newPassword;
					_client.sendMessage(newPassword);
					if (_client.readMessage() == "1") {
						cout << "The password was successfully changed" << endl;
					}
					else {
						cout << "Error! The password has not been changed" << endl;
					}
					break;
				case 7:
					cout << "\n\t*Sign out*" << endl;
					cout << "User offline" << endl;
					_isUserOnline = false;
					break;
				case 0:
					cout << "\n\t*Exit Chat*" << endl;					
					_connection = false;
					_client.closeClient();
					break;
				default:
					break;
				}
			}
		}
	}

private:
	Client _client;
	string _ip;		
	uint16_t _port;			
	bool _connection;
	bool _isUserOnline;			
	string _currentUser;	
};
