#pragma once
#include "neo_output.h"
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <winsock2.h>

class NeoServer : public INeoOutput
{
private:
    std::vector<SOCKET> client_sockets_;
    std::vector<sockaddr_in> client_addrs_;
    SOCKET server_socket_;

    constexpr static int SERVER_PORT = 12345;
    constexpr static int MAX_CLIENTS = 5;

  public:
    NeoServer();
    void outputDialogue(const std::string &id, const std::string &text) const override;
    void Start();
};
