#define WIN32_LEAN_AND_MEAN

#include "WinSock2.h"
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")


class WindowsServer {
private:
	int port;
	PCSTR IP;

	void shutDownConnection(SOCKET& socketToClose, ADDRINFO* addr) {
		closesocket(socketToClose);
		socketToClose = INVALID_SOCKET;
		freeaddrinfo(addr);
		WSACleanup();
	}

public:
	WindowsServer(int inPort, PCSTR inIP) : port(inPort),IP(inIP) {}

	int StartServer() {
		WSADATA wsData;
		WORD ver = MAKEWORD(2, 2);
		int wsOk = WSAStartup(ver, &wsData);

		if (wsOk != 0) {
			std::cerr << "Can't initialize winsock. Quitting..." << std::endl;
			return 1;
		}

		SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
		if (listening == INVALID_SOCKET)
		{
			std::cerr << "Can't create socket, Err #" << WSAGetLastError() << std::endl;
			WSACleanup();
			return 2;
		}

		sockaddr_in hint;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		hint.sin_addr.S_un.S_addr = INADDR_ANY;

		int result = bind(listening, (sockaddr*)&hint, sizeof(hint));
		if (result == SOCKET_ERROR) {
			std::cout << "Binding socket failed" << std::endl;
			shutDownConnection(listening, (ADDRINFO*)&hint);
				
			return 10;
		}

		result = listen(listening, SOMAXCONN);

		if (result == SOCKET_ERROR) {
			std::cout << "Listening socket failed" << std::endl;
			shutDownConnection(listening, (ADDRINFO*)&hint);

			return 9;
		}

		sockaddr_in client;
		int clientSize = sizeof(client);

		SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
		if (clientSocket == INVALID_SOCKET) {
			std::cout << "Accepting socket failed" << std::endl;
			shutDownConnection(clientSocket, (ADDRINFO*)&hint);

			return 2;
		}

		char host[NI_MAXHOST];
		char service[NI_MAXHOST];

		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);

		if (getnameinfo((SOCKADDR*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
			std::cout << host << " Connected on port " << service << std::endl;

		}
		else {
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			std::cout << host << " connected on port " << ntohs(client.sin_port) << std::endl;

		}

		closesocket(listening);

		char buf[4096];
		while (true) {
			ZeroMemory(buf, 4096);

			int bytesReceived = recv(clientSocket, buf, 4096, 0);
			if (bytesReceived == SOCKET_ERROR) {
				std::cerr << "Error in recv(). Quitting" << std::endl;
				break;
			}
			if (bytesReceived == 0) {
				std::cout << "Client disconnected " << std::endl;
				break;
			}
			send(clientSocket, buf, bytesReceived + 1, 0);
			std::cout << "Message from client: " << buf << std::endl;
		}

		closesocket(clientSocket);
		WSACleanup();

		return 0;
		//WSADATA wsaData;
		//ADDRINFO hints;
		//ADDRINFO* addrResult = nullptr;
		//SOCKET ClientSocket = INVALID_SOCKET;
		//SOCKET ListenSocket = INVALID_SOCKET;
		//int result;

		//const char* sendBuffer = "Hello from Server!";
		//char recvBuffer[512];
  //    	
		//result = WSAStartup(MAKEWORD(2, 2), &wsaData);

		//if (result != 0) {
		//	std::cout << "WSAStartup failed, result = " << result << std::endl;
		//	return 1;
		//}

		//ZeroMemory(&hints,sizeof(hints));
		//hints.ai_family = AF_INET;
		//hints.ai_socktype = SOCK_STREAM;
		//hints.ai_protocol = IPPROTO_TCP;
		//hints.ai_flags = AI_PASSIVE;

		//result = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &addrResult);
		//if (result != 0) {
		//	std::cout << "getaddrinfo failed with error: " << result << std::endl;
		//	WSACleanup();
		//	return 1;
		//}
		//
		//ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
		//if (ListenSocket == INVALID_SOCKET) {
		//	std::cout << "Socket creation failed" << std::endl;
		//	shutDownConnection(ListenSocket, addrResult);
		//	return 1;
		//}

		//

		//result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
		//if (result == SOCKET_ERROR) {
		//	std::cout << "Binding socket failed" << std::endl;
		//	shutDownConnection(ListenSocket, addrResult);
		//	
		//	return 1;
		//}

		//result = listen(ListenSocket, SOMAXCONN);
		//if (result == SOCKET_ERROR) {
		//	std::cout << "Listening socket failed" << std::endl;
		//	shutDownConnection(ListenSocket, addrResult);

		//	return 1;
		//}

		//ClientSocket = accept(ListenSocket, NULL, NULL);
		//if (ClientSocket == INVALID_SOCKET) {
		//	std::cout << "Accepting socket failed" << std::endl;
		//	shutDownConnection(ListenSocket, addrResult);

		//	return 1;
		//}
		//
		//
		//do {
		//	ZeroMemory(recvBuffer, 512);
		//	result = recv(ClientSocket, recvBuffer, 512, 0);

		//	if (result > 0) {
		//		std::cout << "Received: " << result << " bytes" << std::endl;
		//		std::cout << "Received data: " << recvBuffer << std::endl;

		//		// Обработка сообщения от клиента
		//		std::string clientMessage(recvBuffer, result);  // Преобразование в строку для удобства работы

		//		// Добавьте вашу логику обработки сообщения здесь
		//		std::cout << "Processing client message: " << clientMessage << std::endl;
		//	}
		//	else if (result == 0) {
		//		
		//	}
		//	else {
		//		std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
		//		shutDownConnection(ClientSocket, addrResult);
		//		return false;
		//	}

		//} while (result >= 0);

		//result = shutdown(ClientSocket, SD_SEND);
		//if (result == SOCKET_ERROR) {
		//	std::cout << "Shut down socket failed" << std::endl;
		//	shutDownConnection(ClientSocket, addrResult);
		//	return 1;
		//}

		//closesocket(ListenSocket);
		//shutDownConnection(ClientSocket, addrResult);
		//return 0;
	}
};

class LinuxServer {
public:
	LinuxServer() {


	}
};