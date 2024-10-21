module;

#include <Windows.h>
#include <fmt/format.h>

export module dialogue;

import <chrono>;
import <iostream>;
import <map>;
import <memory>;
import <optional>;
import <string>;
import <vector>;
import <thread>;

import neo_output;

export class TempOutput : public INeoOutput {
  public:
    void outputDialogue(const std::string& id, const std::string& text) override {
        std::cout << id << ": " << text << std::endl;
    }
};

export class Dialogue {
  public:
    std::string id_;
    std::string encoding_;
    std::vector<char> buffer_;
    INeoOutput& output_;
    std::chrono::milliseconds flush_timeout_;
    std::chrono::time_point<std::chrono::steady_clock> last_received_time_;

    Dialogue() = delete;
    Dialogue(std::string id, std::string encoding, INeoOutput& output,
             const std::chrono::milliseconds& flush_timeout = std::chrono::milliseconds(500))
        : id_(id), encoding_(encoding), output_(output), flush_timeout_(flush_timeout) {}

    void PushText(char buffer) {
        buffer_.push_back(buffer);
        last_received_time_ = std::chrono::steady_clock::now();
    }

    bool NeedFlush() {
        return last_received_time_ + flush_timeout_ < std::chrono::steady_clock::now();
    }

    std::string GetHexText() {
        std::string strDebug;
        for (size_t i = 0; i < buffer_.size(); i++) {
            strDebug += fmt::format("{:02x} ", (unsigned char)buffer_[i]);
        }
        return strDebug;
    }

    std::string GetUTF8Text() {
        if (encoding_ == "UTF-8") {
            return std::string(buffer_.begin(), buffer_.end());
        }

        if (encoding_ == "UTF-16LE") {
            const wchar_t* utf16_str = reinterpret_cast<const wchar_t*>(buffer_.data());
            int utf16_len = buffer_.size() / sizeof(wchar_t);

            int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str, utf16_len, nullptr, 0, nullptr, nullptr);
            std::string utf8_str(utf8_size, 0);
            WideCharToMultiByte(CP_UTF8, 0, utf16_str, utf16_len, &utf8_str[0], utf8_size, nullptr, nullptr);

            return utf8_str;
        }

        if (encoding_ == "Shift-JIS") {
            const char* shift_jis_str = buffer_.data();
            int shift_jis_len = buffer_.size();

            int utf16_size = MultiByteToWideChar(932, 0, shift_jis_str, shift_jis_len, nullptr, 0);
            std::wstring utf16_str(utf16_size, 0);
            MultiByteToWideChar(932, 0, shift_jis_str, shift_jis_len, &utf16_str[0], utf16_size);

            int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_size, nullptr, 0, nullptr, nullptr);
            std::string utf8_str(utf8_size, 0);
            WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_size, &utf8_str[0], utf8_size, nullptr, nullptr);

            return utf8_str;
        }

        return "";
    }

    void Flush() {
        if (!buffer_.empty() && NeedFlush()) {
            auto outputText = GetUTF8Text();
            output_.outputDialogue(id_, outputText);
            buffer_.clear();
        }
    }
};

export class Sink {
  private:
    INeoOutput& output_;
    std::vector<std::unique_ptr<Dialogue>> dialogues_;
    std::chrono::milliseconds flush_timeout_;
    inline static Sink* instance_;

  public:
    Sink(INeoOutput& output, unsigned int flush_timeout)
        : output_(output), flush_timeout_(flush_timeout) {}

    static void Init(INeoOutput& output, unsigned int flush_timeout) {
        instance_ = new Sink(output, flush_timeout);
        instance_->Start();
    }

    static void Push(std::string id, std::string encoding, char buffer) {
        instance_->PushTextToDialogue(id, encoding, buffer);
    }

    void PushTextToDialogue(std::string id, std::string encoding, char buffer) {
        for (const auto& dialogue : dialogues_) {
            if (dialogue->id_ == id) {
                dialogue->PushText(buffer);
                return;
            }
        }

        auto new_dialogue = std::make_unique<Dialogue>(id, encoding, output_, flush_timeout_);
        new_dialogue->PushText(buffer);
        dialogues_.push_back(std::move(new_dialogue));
    }

    void Start() {
        std::thread([this] {
          while (true) {
              std::this_thread::sleep_for(std::chrono::milliseconds(10));
              for (auto& dialogue : dialogues_) {
                  dialogue->Flush();
              }
          }
        }).detach();
    }
};
