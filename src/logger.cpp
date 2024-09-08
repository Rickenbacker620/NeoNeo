#include "logger.h"
#include <iostream>

void Logger::outputDialogue(const std::string& id, const std::string& text) const {
    std::cout << "Dialogue " << id << ": " << text << std::endl;
}