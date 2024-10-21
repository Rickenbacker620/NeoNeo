export module logger;

import <iostream>;

// Import the local module for INeoOutput
import neo_output;

export class Logger : public INeoOutput {
  public:
    void outputDialogue(const std::string& id, const std::string& text) override {
        std::cout << "Dialogue " << id << ": " << text << std::endl;
    }
};
