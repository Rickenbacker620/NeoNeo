export module pcengine;

import engine_base;
import hook_param;
import hook;

export class PCEngine : public Engine
{
  public:
    PCEngine(MessageHandler&& control_out, MessageHandler&& lines_out)
        : Engine{"PCEngine", std::move(control_out), std::move(lines_out)}
    {
        hooks_.emplace_back("HW2@0:gdi32.dll:GetGlyphOutlineW"_hcode);
    }
};

