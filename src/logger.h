#pragma once

#include "neo_output.h"

class Logger : public INeoOutput {
public:
    void outputDialogue(const std::string& id, const std::string& text) override;
};