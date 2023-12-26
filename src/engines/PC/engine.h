#pragma once
#include "../engine_base.h"

class PCEngine : public Engine
{
    using HookConfigList = std::vector<std::pair<std::string, PostHookParam>>;

  private:
    HookConfigList kernel32_funcs_{
        {"lstrlenA", {}},
        {"lstrcpyA", {}},
        {"lstrcpyW", {}}
    };

  public:
    PCEngine();
    ~PCEngine();
    static Engine *Match();

    REGISTER_ENGINE_MATCH
};