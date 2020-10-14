// Compilation Command: g++ "Simple Socket - Client.cpp" -o "Simple Socket - Client" -lwsock32

#include <iostream>
#include <winsock2.h>

const char* server_address = "127.0.0.1";
const u_short server_port = 8888;

int main()
{
	std::cout << "Initialising Winsock...\n";
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa))
	{
		std::cout << "Initialisation failed with error code: " << WSAGetLastError() << '\n';
		return 1;
	}

	std::cout << "Initialised\n";

	SOCKET s;
	// Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		std::cout << "Socket creation failed with error code: " << WSAGetLastError() << '\n';
		return 1;
	}

	std::cout << "Socket created\n";

	sockaddr_in server;
	server.sin_addr.S_un.S_addr = inet_addr(server_address);
	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);

	// Connect to remote server
	if (connect(s, (sockaddr *) &server, sizeof(server)) < 0)
	{
		std::cout << "Connect failed with error code: " << WSAGetLastError() << '\n';
		return 1;
	}

	std::cout << "Connected\n";
}
