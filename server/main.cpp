#include "Server.h"
#include "Database.h"
#include "Chat.h"


int main() {
	string ip{ "127.0.0.7" };
	uint16_t port{ 7777 };
	string dbHost{ "localhost" };
	string dbUser{ "bolshakov" };
	string dbPassword{ "5Jerf8dqsql!" };
	string dbName{ "testdb" };

	Chat chat(ip, port, dbHost, dbUser, dbPassword, dbName);

	chat.Start();

	return 0;
}