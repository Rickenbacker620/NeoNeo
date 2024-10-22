export module pcengine;

import engine_base;
import hook_param;
import hook;

export class PCEngine : public Engine
{
  public:
    PCEngine() : Engine{"PC"}
    {
        hooks_.emplace_back("HW2@0:gdi32.dll:GetGlyphOutlineW"_hcode);
    }
};

