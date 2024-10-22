export module engine_base;

import <vector>;
import <iostream>;

import hook;

export class Engine
{
  protected:
    const char *name_;
    std::vector<Hook> hooks_;

  public:
    Engine(const char *name) : name_(name) {};

    virtual ~Engine()
    {
        std::cout << "Detaching ----" << name_ << std::endl;
        for (auto &hook : hooks_)
        {
            hook.Detach();
        }
    }

    void AttachHooks()
    {
        std::cout << "Injectinggg ----" << name_ << std::endl;
        for (auto &hook : hooks_)
        {
            hook.Attach();
            std::cout << hook.GetName() << "attached" << std::endl;
        }
    }

};
