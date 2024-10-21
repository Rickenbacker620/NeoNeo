#pragma once
#include "../hook.h"
#include <zmq.hpp>
#include <thread>

#define REGISTER_ENGINE_MATCH                                                                                          \
    inline static bool _ = [] {                                                                                        \
        Engine::matches.push_back(&Match);                                                                             \
        return true;                                                                                                   \
    }();

class HookController
{
  private:
    int port_ = 18888;

  public:
    void Start()
    {
        std::thread([this] {
            zmq::context_t context(1);
            zmq::socket_t socket(context, ZMQ_REP);
            socket.bind("tcp://*:" + std::to_string(port_));

            while (true)
            {
                zmq::message_t request;
                socket.recv(request, zmq::recv_flags::none);

                std::string_view req_str(static_cast<char *>(request.data()), request.size());
                std::cout << "Received: " << req_str << std::endl;

                zmq::message_t reply(5);
                std::memcpy(reply.data(), "World", 5);
                socket.send(reply, zmq::send_flags::none);
            }
        }).detach();
    }
};

class Engine
{
  protected:
    const char *name_;
    std::vector<Hook> hooks_;
    std::unique_ptr<HookController> controller_;

  public:
    Engine(const char *name) : name_(name) {};
    virtual ~Engine();

    void AttachHooks();
    void StartListenController();

    using EngineMatch = Engine *(*)();

    static inline std::vector<EngineMatch> matches;

    static Engine *DeduceEngineType()
    {
        for (const auto &match : matches)
        {
            if (Engine *engine = match())
            {
                return engine;
            }
        }
        return nullptr;
    }
};
