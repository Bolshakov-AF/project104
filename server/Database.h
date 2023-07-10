#pragma once

#include <mysql.h>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

class Database {
public:
	Database() {};

	~Database()
	{	
		mysql_close(_mysql);
	}
	
	void connect(const string& host, const string& user, const string& password, const string& name) {

		_dbHost = host;
		_dbUser = user;
		_dbPassword = password;
		_dbName = name;

		_mysql = mysql_init(NULL);

		if (_mysql == NULL) {			
			cout << "Error: can't create MySQL descriptor" << endl;
			return;
		}
		
		if (!mysql_real_connect(_mysql, _dbHost.c_str(), _dbUser.c_str(), _dbPassword.c_str(), NULL, 0, NULL, 0)) {			
			cout << "Error: can't create MySQL descriptor" << mysql_error(_mysql) << endl;
			return;
		}
		else {			
			cout << "*Successful connection to the database." << endl;
		}
		
		string query = "create database if not exists " + _dbName;
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			cout << "*A database is created or found. " << endl;
		}
				
		_mysql = mysql_init(NULL);
		if (!mysql_real_connect(_mysql, _dbHost.c_str(), _dbUser.c_str(), _dbPassword.c_str(), _dbName.c_str(), 0, NULL, 0)) {			
			cout << "Error: can't connect to database " << _dbName << "  " << mysql_error(_mysql) << endl;
			return;
		}
		else {			
			cout << "*Database connection - " << _dbName << " - success." << endl;
		}

		createTables(); 

		mysql_set_character_set(_mysql, "utf8mb4");
	}

	bool checkUserByLogin(string& login) {
		string query = "select id from Users where login='" + login + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			if (row = mysql_fetch_row(res))
				return true;
			else
				return false;
		}
	}

	void addUser(const string& name, const string& login, const string& password) {
		string query = "insert into Users(id, name, login, password) values(default, '" +
			name + "', '" + login + "', '" + password + "')";
		int res = mysql_query(_mysql, query.c_str());
		if (res == 0) {
			cout << "User with login " << login << " successfully added" << endl;
		}
		else {
			cout << "Error when adding a user with a login " << login << " - " << mysql_error(_mysql) << endl;
		}
	}

	bool checkUserByLoginAndPassword(const string& login, const string& password) {
		string query = "select password from Users where login='" + login + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				if (row[0] == password)
					return true;
				else
					return false;
			}
		}
	}

	vector<string> allUsersInChat() {
		vector<string> users{};
		string query = "select a login from the Users list";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				users.push_back(row[0]);
			}
		}
		return users;
	}

	string getUserId(const string& login) {
		string query = "select id from Users where login='" + login + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			row = mysql_fetch_row(res);
			return row[0];
		}
	}

	string getUserLoginById(const string& id) {
		string query = "select login from Users where id='" + id + "'";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			row = mysql_fetch_row(res);
			return row[0];
		}
	}

	void addPrivateMessage(const string& sender, const string& reciever, const string& text) {
		string senderId = getUserId(sender);
		string recieverId = getUserId(reciever);
		string query = "insert into MessagesPrivate(id, senderId, recieverId, text) values(default, '" +
			senderId + "', '" + recieverId + "', '" + text + "')";
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			cout << "The message was successfully added." << endl;
		}
		else {
			cout << "Error adding a message - " << mysql_error(_mysql) << endl;
		}
	}

	void addMessageToAll(const string& sender, const string& text) {
		string senderId = getUserId(sender);
		string query = "insert into MessagesToAll(id, senderId, text) values(default, '" +
			senderId + "', '" + text + "')";
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			cout << "The message was successfully added." << endl;
		}
		else {
			cout << "Error adding a message - " << mysql_error(_mysql) << endl;
		}
	}

	vector<string> showPrivateMessages(const string& reciever) {
		vector<string> messagesPrivate;
		string message{};
		string query = "select * from MessagesPrivate";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				if (getUserLoginById(row[2]) == reciever) {
					message = "From: " + getUserLoginById(row[1]) + "\tTo: Me" + "\nText: " + row[3];
					messagesPrivate.push_back(message);
				}
			}
		}
		return messagesPrivate;
	}

	vector<string> showMessagesToAll() {
		vector<string> messagesToAll{};
		string message{};
		string query = "select * from MessagesToAll";
		mysql_query(_mysql, query.c_str());
		MYSQL_RES* res;
		MYSQL_ROW row;
		if (res = mysql_store_result(_mysql)) {
			while (row = mysql_fetch_row(res)) {
				message = "From: " + getUserLoginById(row[1]) + "\tTo: All" + "\nText: " + row[2];
				messagesToAll.push_back(message);
			}
		}
		return messagesToAll;
	}

	bool changePassword(const string& login, const string& newPassword) {
		string id = getUserId(login);
		string query = "update Users set password=" + newPassword + " where id=" + id + "";
		int result = mysql_query(_mysql, query.c_str());
		if (result == 0) {
			cout << "The password was successfully changed." << endl;
			return true;
		}
		else {
			cout << "Error change a password - " << mysql_error(_mysql) << endl;
			return false;
		}
	}

private:
	MYSQL* _mysql;				
	string _dbHost;		
	string _dbUser;		
	string _dbPassword;	
	string _dbName;		

	void createTables() {
		int result{};		
		result = mysql_query(_mysql, "create table if not exists Users"
			"(id int auto_increment primary key, " 
			"login varchar(255) not null, "
			"password varchar(255) not null)");
		if (result == 0) {
			cout << "*Table Users is created or finded." << endl;
		}
		else {
			cout << "Table Users is not created or not finded." << endl;
		}
		
		result = mysql_query(_mysql, "create table if not exists MessagesPrivate"
			"(id int auto_increment primary key,"
			"senderId int references Users(id), "
			"recieverId int references Users(id), "
			"text varchar(1024))");
		if (result == 0) {
			cout << "*Table MessagesPrivate is created or finded." << endl;
		}
		else {
			cout << "Table MessagesPrivate is not created or not finded." << endl;
		}
		
		result = mysql_query(_mysql, "create table if not exists MessagesToAll"
			"(id int auto_increment primary key,"
			"senderId int references Users(id), "
			"text varchar(1024))");
		if (result == 0) {
			cout << "*Table MessagesToAll is created or finded." << endl;
		}
		else {
			cout << "Table MessagesToAll is not created or not finded." << endl;
		}
	}
};
