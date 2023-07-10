#include "Client.h"
#include "Chat.h"
using namespace std;

int main() {

	Sleep(1000);

	string ip{ "127.0.0.7" };
	uint16_t port{ 7777 };

	Chat chat(ip, port);
	chat.Start();

	return 0;
}

