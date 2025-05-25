module;
#include <zmq.hpp>
export module console;

import <iostream>;
import <thread>;
import <vector>;
import <string>;


// Singleton class for logging to console
export class ConsoleOut {
  public:
  static void log(const std::string &msg) {
      std::cout << msg << std::endl;
  }
};

export class NeoServer {
  private:
    zmq::context_t context_;
    zmq::socket_t publisher_;

    constexpr static int SERVER_PORT = 12345;
    inline static NeoServer* instance_;

  public:
    NeoServer()
        : context_(1),
          publisher_(context_, zmq::socket_type::pub)
    {
        std::string bind_address = "tcp://*:" + std::to_string(SERVER_PORT);
        publisher_.bind(bind_address);
        ConsoleOut::log("Server started on port " + std::to_string(SERVER_PORT));
    }

    static void Init() {
        if (!NeoServer::instance_) {
            instance_ = new NeoServer();
        }
    }

    static NeoServer& GetInstance() {
        return *NeoServer::instance_;
    }

    void output(std::string_view msg) {
        publisher_.send(zmq::buffer(msg), zmq::send_flags::none);
    }
};

export class LinesOut
{
  public:
    static void log(const std::string& hookName, const std::string &msg) {
      auto message = std::format("[{:s}] {:s}", hookName, msg);
      NeoServer::GetInstance().output(message);
    }
};
