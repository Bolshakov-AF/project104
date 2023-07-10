#pragma once

#include "Database.h"
#include "Server.h"
#include <fstream>
#include <string>
using namespace std;


class Chat final {
public:
	Chat(const string& ip, const uint16_t port,
		const string& dbHost, const string& dbUser, const string& dbPassword, const string& dbName) :
		_isUserOnline(false), _connection(false), _ip(ip), _port(port),
		_dbHost(dbHost), _dbUser(dbUser), _dbPassword(dbPassword), _dbName(dbName) {

		_dataBase.connect(_dbHost, _dbUser, _dbPassword, _dbName);
		if (_server.initSock(_ip, _port)) _server.StartServer();
	}

	~Chat() = default;

	void Start() {
		_connection = true;
		while (_connection)
		{
			string message{};
			message = _server.readMessage();
			string name, login, password;
			vector<string> users{};
			bool loginS = false;
			bool passwordS = false;
			string to, text;
			vector<string> userMessages;
			string newPassword{};

			cout << "User: " << message << endl;
			if (!_isUserOnline && (message == "1" || message == "2" || message == "0"))
				_server.sendMessage("1 - Operation is correct");
			else if (_isUserOnline && message == "3" || message == "4" || message == "5" || message == "6" || message == "7")
				_server.sendMessage("1 - Operation is correct");
			else
				_server.sendMessage("2 - Operation is incorrect");

			switch (stoi(message))
			{
			case 1:
				cout << "Enter LOGIN menu" << endl;
				loginS = false;
				passwordS = false;
				while (!loginS) {
					login = _server.readMessage();
					if (_dataBase.checkUserByLogin(login)) {
						loginS = true;
						cout << "User's login: " << login << endl;
						_server.sendMessage("1");
					}
					else {
						_server.sendMessage("0");
					}
					while (!passwordS) {
						password = _server.readMessage();
						if (_dataBase.checkUserByLoginAndPassword(login, password)) {
							_currentUser = login;
							_isUserOnline = true;
							passwordS = true;
							cout << "User's password is correct!" << endl;
							_server.sendMessage("1");
						}
						else
							_server.sendMessage("0");
					}
				}
				break;
			case 2:
				cout << "Enter SIGNUP menu" << endl;
				name = _server.readMessage();
				cout << "User's name: " << name << endl;
				while (!loginS) {
					message = _server.readMessage();
					if (_dataBase.checkUserByLogin(message))
						_server.sendMessage("1");
					else {
						login = message;
						cout << "User's login: " << login << endl;
						loginS = true;
						_server.sendMessage("0");
					}
				}
				password = _server.readMessage();
				cout << "User's password: " << password << endl;
				_dataBase.addUser(name, login, password);
				_server.sendMessage("Adding a user was successful!");
				_currentUser = login;
				_isUserOnline = true;
				break;
			case 3:
				cout << "Enter SHOW ALL USERS IN CHAT menu" << endl;
				users = _dataBase.allUsersInChat();
				_server.sendMessage(to_string(users.size()));
				_server.readMessage();
				for (int i = 0; i < users.size(); i++) {
					_server.sendMessage(users[i]);
					_server.readMessage();
				}
				break;
			case 4:
				cout << "Enter SHOW ALL MESSAGES menu" << endl;
				message = _server.readMessage();
				switch (stoi(message))
				{
				case 1:
					userMessages.clear();
					userMessages = _dataBase.showMessagesToAll();
					_server.sendMessage(to_string(userMessages.size()));
					if (userMessages.size() == 0)
						cout << "It's no messages to all in database!" << endl;
					else {
						cout << "Messages to all: " << userMessages.size() << endl;
						_server.readMessage();
						for (int i = 0; i < userMessages.size(); i++) {
							_server.sendMessage(userMessages[i]);
							_server.readMessage();
						}
					}
					break;
				case 2:
					userMessages.clear();
					userMessages = _dataBase.showPrivateMessages(_currentUser);
					_server.sendMessage(to_string(userMessages.size()));
					if (userMessages.size() == 0)
						cout << "It's no messages to user with login" << _currentUser << "in database!" << endl;
					else {
						cout << "Messages to user with login" << _currentUser << ": " << userMessages.size() << endl;
						_server.readMessage();
						for (int i = 0; i < userMessages.size(); i++) {
							_server.sendMessage(userMessages[i]);
							_server.readMessage();
						}
					}
					break;
				default:
					break;
				}
				break;
			case 5:
				cout << "Enter ADD MESSAGE TO CHAT menu" << endl;
				loginS = false;
				while (!loginS) {
					to = _server.readMessage();
					if (!(to == "all")) {
						if (_dataBase.checkUserByLogin(to)) {
							loginS = true;
							cout << "Message to: " << to << endl;
							_server.sendMessage("1");
							text = _server.readMessage();
							_dataBase.addPrivateMessage(_currentUser, to, text);
						}
						else
							_server.sendMessage("0");
					}
					else {
						loginS = true;
						text = _server.readMessage();
						_dataBase.addMessageToAll(_currentUser, text);
					}
				}
				break;
			case 6:
				cout << "Enter CHANGE PASSWORD menu" << endl;
				newPassword = _server.readMessage();
				if (_dataBase.changePassword(_currentUser, newPassword)) {
					_server.sendMessage("1");
				}
				else {
					_server.sendMessage("0");
				}
				break;
			case 7:
				cout << "Enter SIGN OUT menu" << endl;
				cout << "User offline" << endl;
				_isUserOnline = false;
				break;
			case 0:
				cout << "Enter EXIT menu" << endl;
				cout << "\n\t*Exit Chat*" << endl;
				_connection = false;
				_server.closeServer();
				break;
			default:
				break;
			}
		}
	}

private:
	string _currentUser;	
	bool _isUserOnline;			
	bool _connection;

	Server _server;				
	string _ip;		
	uint16_t _port;			

	Database _dataBase;			
	string _dbHost;		
	string _dbUser;		
	string _dbPassword;	
	string _dbName;		
};
