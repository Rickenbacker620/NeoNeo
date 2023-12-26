#pragma once

#define REGISTER_ENGINE_MATCH                                                                                          \
    inline static bool _ = [] {                                                                                        \
        Engine::matches.push_back(&Match);                                                                             \
        return true;                                                                                                   \
    }();

struct HookAddress
{
    const char *module;
    const char *function;
    const uintptr_t offset;
    void *GetAddress()
    {
        auto handle = GetModuleHandle(module);
        uintptr_t result = 0;
        if (function)
        {
            result = reinterpret_cast<uintptr_t>(GetProcAddress(handle, function));
        }
        else
        {
            result = reinterpret_cast<uintptr_t>(handle);
        }
        return reinterpret_cast<void *>(result + offset);
    }
};

struct PostHookParam
{
    const uintptr_t offset;
    const uint16_t index;
    const uint16_t split;
    const uint32_t flag;
};

class Hook
{
  private:
    // pre-hook variables
    const char *module_;
    const char *function_;
    const uintptr_t offset_;

    const std::string name_;
    void *address_;

    uintptr_t data_offset_;
    uint16_t index_;
    uint16_t split_;

    // post-hook variables
    BYTE trampoline[40];
    void *GetHookAddress();
    const std::string GetHookName();

  public:
    Hook(const char *module, const char *function, const uintptr_t offset = 0);
    void Send(uintptr_t dwDatabase);
    bool Attach();
};

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
