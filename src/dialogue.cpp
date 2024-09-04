#include "dialogue.h"
#include <fmt/format.h>
#include <optional>

void Dialogue::PushTextToDialogue(std::string id, char buffer)
{
    // std::cout << "currently " << dialogues_.size() << " dialogues" << std::endl;
    // std::cout << "Trying to find dialogue with id: " << id << std::endl;

    // auto dialogue = GetDialogue(id);
    // if (dialogue == std::nullopt) {
    //     auto new_dialogue = Dialogue(id);
    //     std::cout << "Creating new dialogue with id: " << id << std::endl;
    //     new_dialogue.PushText(buffer);
    //     dialogues_.push_back(new_dialogue);
    // } else {
    //     std::cout << "Found dialogue with id: " << id << std::endl;
    //     dialogue.value().PushText(buffer);

    // }

    for (const auto &dialogue : dialogues_)
    {
        if (dialogue->id_ == id)
        {
            dialogue->PushText(buffer);
            return;
        }
    }

    auto new_dialogue = std::make_unique<Dialogue>(id);
    new_dialogue->PushText(buffer);
    new_dialogue->Start();
    dialogues_.push_back(std::move(new_dialogue));
    return;
}

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

void Dialogue::Start()
{
    std::thread([this] {
        while (true)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            Flush();
        }
    }).detach();
}

std::string Dialogue::GetUTF8Text()
{
    if (encoding_ == "UTF-8")
    {
        // If the encoding is already UTF-8, simply return the string
        return std::string(buffer_.begin(), buffer_.end());
    }

    if (encoding_ == "UTF-16")
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

Dialogue::Dialogue(std::string id) : id_(id)
{
    encoding_ = id.substr(id.find_last_of('\x02') + 1, id.size());
}

void Dialogue::Flush()
{
    if (!buffer_.empty() && NeedFlush() && id_.find("GetGlyphOutline") != std::string::npos)
    {
        std::cout << id_ << ": " << GetUTF8Text() << std::endl;
        buffer_.clear();
    }
}