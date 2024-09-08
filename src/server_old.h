#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>

class NeoServer
{
  private:
    std::vector<SOCKET> client_sockets_;
    std::vector<sockaddr_in> client_addrs_;
    SOCKET server_socket_;

    constexpr static int SERVER_PORT = 12345;
    constexpr static int MAX_CLIENTS = 5;

  public:
    NeoServer();
    void Run();
    void Send(std::string_view text);
};

void NeoServer::Send(std::string_view text)
{
    for (const auto &s : client_sockets_)
    {
        send(s, text.data(), text.length(), 0);
    }
}

void NeoServer::Run()
{
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
}

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

    std::thread([this] { Run(); }).detach();
}

inline NeoServer g_server;