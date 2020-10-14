// Compilation Command: g++ "Simple Socket - Server.cpp" -o "Simple Socket - Server" -lwsock32

#include <iostream>
#include <winsock2.h>

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
	server.sin_addr.S_un.S_addr = INADDR_ANY;
	server.sin_family = AF_INET;
	server.sin_port = htons(server_port);

	// Bind
	if (bind(s, (sockaddr *) &server, sizeof(server)) == SOCKET_ERROR)
	{
		std::cout << "Bind failed with error code: " << WSAGetLastError() << '\n';
		return 1;
	}

	std::cout << "Bind done\n";

	// Listen to incoming connections
	listen(s, 3);

	// Accept an incoming connection
	std::cout << "Waiting for an incoming connection...\n";
	sockaddr_in client;
	int c = sizeof(sockaddr_in);
	if (accept(s, (sockaddr *) &client, &c) == INVALID_SOCKET)
	{
		std::cout << "Accept failed with error code: " << WSAGetLastError() << '\n';
		return 1;
	}

	std::cout << "Connection accepted\n";
}
