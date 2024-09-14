#pragma once
#include "neo_output.h"
#include <chrono>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

class TempOutput : public INeoOutput
{
  public:
    void outputDialogue(const std::string &id, const std::string &text) override
    {
        std::cout << id << ": " << text << std::endl;
    }
};

class Dialogue
{
  public:
    std::string id_;
    std::string encoding_;
    std::vector<char> buffer_;
    INeoOutput &output_;
    std::chrono::milliseconds flush_timeout_;
    std::chrono::time_point<std::chrono::steady_clock> last_received_time_;

    Dialogue() = delete;
    Dialogue(std::string id, std::string encoding, INeoOutput &output,
             const std::chrono::milliseconds &flush_timeout = std::chrono::milliseconds(500));

    bool NeedFlush();
    void Flush();
    void PushText(char buffer);

    std::string GetHexText();
    std::string GetUTF8Text();
};

class DialoguePool
{
  private:
    INeoOutput &output_;
    std::vector<std::unique_ptr<Dialogue>> dialogues_;
    std::chrono::milliseconds flush_timeout_;

  public:
    static DialoguePool getInstance(INeoOutput &output, unsigned int flush_timeout)
    {
        return DialoguePool(output, flush_timeout);
    }
    DialoguePool(INeoOutput &output, unsigned int flush_timeout);
    void PushTextToDialogue(std::string id, std::string encoding, char buffer);
    void Start();
};