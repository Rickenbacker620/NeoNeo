#pragma once
#include <string>

class INeoOutput {
public:
    virtual void outputDialogue(const std::string& id, const std::string& text) const = 0;
};