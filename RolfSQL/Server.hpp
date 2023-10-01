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

	bool StartServer() {
		WSADATA wsaData;
		ADDRINFO hints;
		ADDRINFO* addrResult = nullptr;
		SOCKET ClientSocket = INVALID_SOCKET;
		SOCKET ListenSocket = INVALID_SOCKET;
		int result;

		const char* sendBuffer = "Hello from Server!";
		char recvBuffer[512];
      	
		result = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (result != 0) {
			std::cout << "WSAStartup failed, result = " << result << std::endl;
			return 1;
		}

		ZeroMemory(&hints,sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		result = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &addrResult);
		if (result != 0) {
			std::cout << "getaddrinfo failed with error: " << result << std::endl;
			WSACleanup();
			return 1;
		}
		
		ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			std::cout << "Socket creation failed" << std::endl;
			shutDownConnection(ListenSocket, addrResult);
			return 1;
		}

		

		result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
		if (result == SOCKET_ERROR) {
			std::cout << "Binding socket failed" << std::endl;
			shutDownConnection(ListenSocket, addrResult);
			
			return 1;
		}

		result = listen(ListenSocket, SOMAXCONN);
		if (result == SOCKET_ERROR) {
			std::cout << "Listening socket failed" << std::endl;
			shutDownConnection(ListenSocket, addrResult);

			return 1;
		}

		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			std::cout << "Accepting socket failed" << std::endl;
			shutDownConnection(ListenSocket, addrResult);

			return 1;
		}
		
		closesocket(ListenSocket);


		
		do {
			ZeroMemory(recvBuffer, 512);
			result = recv(ClientSocket, recvBuffer, 512, 0);

			if (result > 0) {
				std::cout << "Received: " << result << " bytes" << std::endl;
				std::cout << "Received data:" << recvBuffer << std::endl;

				result = send(ClientSocket, sendBuffer, (int)strlen(sendBuffer), 0);
				if (result == SOCKET_ERROR) {
					std::cout << "Failed to send data back" << std::endl;
					shutDownConnection(ClientSocket, addrResult);

					return 1;
				}

			}
			else if (result == 0) {
				std::cout << "Connection closing..." << std::endl;
			}
			else {	
				std::cout << "recv Failed with error" << std::endl;
				shutDownConnection(ClientSocket, addrResult);
				return 1;
			}
			
		} while (result > 0);

		result = shutdown(ClientSocket, SD_SEND);
		if (result == SOCKET_ERROR) {
			std::cout << "Shut down socket failed" << std::endl;
			shutDownConnection(ClientSocket, addrResult);
			return 1;
		}

		shutDownConnection(ClientSocket, addrResult);
		return 0;
	}
};

class LinuxServer {
public:
	LinuxServer() {


	}
};