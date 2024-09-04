#pragma once
#include <map>
#include <vector>
#include <optional>
#include <memory>
#include <chrono>

class Dialogue
{
private:
    std::vector<char> buffer_;
    std::string id_;
    std::string encoding_;

    std::chrono::time_point<std::chrono::steady_clock> last_received_time_;
    inline static std::vector<std::unique_ptr<Dialogue>> dialogues_;
    inline static std::chrono::milliseconds flush_timeout_ = std::chrono::milliseconds(1000);

    void Flush();

public:
    Dialogue() = default;
    Dialogue(std::string id);
    static void PushTextToDialogue(std::string id, char buffer);

    void Start();

    bool NeedFlush();

    void PushText(char buffer);

    std::string GetHexText();
    std::string GetUTF8Text();
};