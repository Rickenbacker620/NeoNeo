export module neo_output;
import <string>;

export class INeoOutput {
public:
    virtual void outputDialogue(const std::string& id, const std::string& text) = 0;
};