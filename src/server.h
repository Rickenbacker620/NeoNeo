#pragma once
#include "neo_output.h"
#include <zmq.hpp>
#include <iostream>
#include <thread>
#include <vector>

class NeoServer : public INeoOutput
{
private:
    zmq::context_t context_;
    zmq::socket_t publisher_;

    constexpr static int SERVER_PORT = 12345;

public:
    NeoServer();
    std::string FormatMessage(const std::string &id, const std::string &text);
    void outputDialogue(const std::string &id, const std::string &text) override;
};
