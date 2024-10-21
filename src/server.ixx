export module server;

import <iostream>;
import <thread>;
import <vector>;
import <string>;
import <zmq.hpp>;
import <fmt/core.h>;

// Import local modules
import neo_output;

export class NeoServer : public INeoOutput {
  private:
    zmq::context_t context_;
    zmq::socket_t publisher_;

    constexpr static int SERVER_PORT = 12345;

    std::string FormatMessage(const std::string& id, const std::string& text) {
        return fmt::format("[{}]{}", id, text);
    }

  public:
    NeoServer()
        : context_(1),
          publisher_(context_, zmq::socket_type::pub)
    {
        std::string bind_address = "tcp://*:" + std::to_string(SERVER_PORT);
        publisher_.bind(bind_address);
        std::cout << "Server is bound to " << bind_address << std::endl;
    }

    void outputDialogue(const std::string& id, const std::string& text) override {
        std::cout << "out" << std::endl;
        auto output = FormatMessage(id, text);
        std::cout << output << std::endl;
        zmq::message_t message(static_cast<void*>(output.data()), output.size());
        publisher_.send(message, zmq::send_flags::none);
    }
};
