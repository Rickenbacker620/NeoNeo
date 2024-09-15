#pragma once
#include <ctre.hpp>
#include <map>
#include "common.h"
#include "hook_param.h"
#include "dialogue.h"

class Hook
{
  private:
    HookParam param_{};
    BYTE trampoline[40]{};

    size_t GetTextLength(address_t base, address_t text_addr) const;
    address_t GetTextAddress(address_t base) const;
    address_t GetTextContext(address_t base) const;

  public:
    Hook(HookParam param) : param_{param}{}

    std::string GetName() const;

    void Send(address_t dwDatabase);

    bool Attach();
    void Detach();
};