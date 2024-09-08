#include "dialogue.h"
#include <Windows.h>
#include <fmt/format.h>
#include <iostream>
#include <optional>
#include <thread>

void Dialogue::PushText(char buffer)
{
    buffer_.push_back(buffer);
    last_received_time_ = std::chrono::steady_clock::now();
}

bool Dialogue::NeedFlush()
{
    return last_received_time_ + flush_timeout_ < std::chrono::steady_clock::now();
}

std::string Dialogue::GetHexText()
{
    std::string strDebug;
    for (size_t i = 0; i < buffer_.size(); i++)
    {
        strDebug += fmt::format("{:02x} ", (unsigned char)buffer_[i]);
    }
    return strDebug;
}

std::string Dialogue::GetUTF8Text()
{
    if (encoding_ == "UTF-8")
    {
        // If the encoding is already UTF-8, simply return the string
        return std::string(buffer_.begin(), buffer_.end());
    }

    if (encoding_ == "UTF-16LE")
    {
        // Convert UTF-16 to UTF-8 using Windows API
        // Assuming the buffer contains little-endian UTF-16 data
        const wchar_t *utf16_str = reinterpret_cast<const wchar_t *>(buffer_.data());
        int utf16_len = buffer_.size() / sizeof(wchar_t);

        int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str, utf16_len, nullptr, 0, nullptr, nullptr);
        std::string utf8_str(utf8_size, 0);
        WideCharToMultiByte(CP_UTF8, 0, utf16_str, utf16_len, &utf8_str[0], utf8_size, nullptr, nullptr);

        return utf8_str;
    }

    if (encoding_ == "Shift-JIS")
    {
        // Convert Shift-JIS to UTF-8 using Windows API
        const char *shift_jis_str = buffer_.data();
        int shift_jis_len = buffer_.size();

        int utf16_size = MultiByteToWideChar(932, 0, shift_jis_str, shift_jis_len, nullptr, 0);
        std::wstring utf16_str(utf16_size, 0);
        MultiByteToWideChar(932, 0, shift_jis_str, shift_jis_len, &utf16_str[0], utf16_size);

        int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_size, nullptr, 0, nullptr, nullptr);
        std::string utf8_str(utf8_size, 0);
        WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), utf16_size, &utf8_str[0], utf8_size, nullptr, nullptr);

        return utf8_str;
    }

    // In case of unknown encoding, return an empty string or handle the error
    return "";
}

Dialogue::Dialogue(std::string id, std::string encoding, const INeoOutput &output,
                   const std::chrono::milliseconds &flush_timeout)
    : id_(id), encoding_(encoding), output_(output), flush_timeout_(flush_timeout)
{
}

void Dialogue::Flush()
{
    if (!buffer_.empty() && NeedFlush())
    {
        output_.outputDialogue(id_, GetUTF8Text());
        buffer_.clear();
    }
}

DialoguePool::DialoguePool(const INeoOutput &output, unsigned int flush_timeout)
    : output_(output), flush_timeout_(flush_timeout)
{
}

void DialoguePool::PushTextToDialogue(std::string id, std::string encoding, char buffer)
{
    for (const auto &dialogue : dialogues_)
    {
        if (dialogue->id_ == id)
        {
            dialogue->PushText(buffer);
            return;
        }
    }

    auto new_dialogue = std::make_unique<Dialogue>(id, encoding, output_, flush_timeout_);
    new_dialogue->PushText(buffer);
    dialogues_.push_back(std::move(new_dialogue));
    return;
}

void DialoguePool::Start()
{
    std::thread([this] {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            for (auto &dialogue : dialogues_)
            {
                dialogue->Flush();
            }
        }
    }).detach();
}