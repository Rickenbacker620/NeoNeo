#pragma once
#include "../engine_base.h"

class KiriKiriEngine : public Engine
{
  private:
  public:
    KiriKiriEngine();
    bool findKiriKiriHook(DWORD fun, DWORD size, DWORD pt, DWORD flag);
};