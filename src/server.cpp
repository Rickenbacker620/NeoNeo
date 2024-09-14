#include "server.h"
#include <fmt/core.h>

NeoServer::NeoServer()
    : context_(1),         // Initialize ZeroMQ context with 1 I/O thread
      publisher_(context_, zmq::socket_type::pub) // Create a PUB socket
{
    // Binding the publisher socket to a TCP port
    std::string bind_address = "tcp://*:" + std::to_string(SERVER_PORT);
    publisher_.bind(bind_address);

    std::cout << "Server is bound to " << bind_address << std::endl;
}

std::string NeoServer::FormatMessage(const std::string &id, const std::string &text)
{
    auto out_str = fmt::format("[{}]{}", id, text);
    return out_str;
}

void NeoServer::outputDialogue(const std::string &id, const std::string &text)
{
    auto output = FormatMessage(id, text);
    std::cout << output << std::endl;
    zmq::message_t message(static_cast<void*>(output.data()), output.size());
    publisher_.send(message, zmq::send_flags::none);
}