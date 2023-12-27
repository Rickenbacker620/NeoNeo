#pragma once
#include "../hook.h"

#define REGIST_ENGINE_MATCH                                                                                          \
    inline static bool _ = [] {                                                                                        \
        Engine::matches.push_back(&Match);                                                                             \
        return true;                                                                                                   \
    }();



class Engine
{
  protected:
    const char *name_;
    std::vector<Hook> hooks_;

  public:
    Engine(const char *name) : name_(name){};
    void AttachHooks();

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
