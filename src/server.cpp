#include "server.h"
#include <iostream>

NeoServer::NeoServer()
{
    WSADATA wsaData;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_size = sizeof(client_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "WSAStartup failed\n";
    }

    server_socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket_ == INVALID_SOCKET)
    {
        std::cerr << "Error creating socket\n";
        WSACleanup();
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_socket_, reinterpret_cast<sockaddr *>(&server_addr), sizeof(server_addr)) == SOCKET_ERROR)
    {
        std::cerr << "Binding error\n";
        closesocket(server_socket_);
        WSACleanup();
    }

    listen(server_socket_, MAX_CLIENTS);

    std::cout << "TCP server started and listening...\n";
}

void NeoServer::Start()
{
    std::cout << "Starting server thread\n";
    std::thread([this] {
        while (true)
        {
            sockaddr_in client_addr;
            int client_addr_size = sizeof(client_addr);
            SOCKET client_socket =
                accept(server_socket_, reinterpret_cast<struct sockaddr *>(&client_addr), &client_addr_size);

            std::cout << "Connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port)
                      << "\n";

            client_sockets_.push_back(client_socket);
        }

        closesocket(server_socket_);

        WSACleanup();
    }).detach();
}

void NeoServer::outputDialogue(const std::string &id, const std::string &text) const
{
    std::cout << "Dialogue " << id << ": " << text << std::endl;
}