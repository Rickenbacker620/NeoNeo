module;
#include <Windows.h>
export module engine_base;

import <vector>;
import <iostream>;
import <functional>;
import <thread>;
import <utility>;

import hook;

std::string BufferToUTF8(const std::vector<char>& buffer, const std::string& encoding) {

    if (encoding == "UTF-8") {
        return std::string(buffer.begin(), buffer.end());
    }

    if (encoding == "UTF-16LE") {
        const wchar_t* utf16_str = reinterpret_cast<const wchar_t*>(buffer.data());
        int utf16_len = buffer.size() / sizeof(wchar_t);

        int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16_str, utf16_len, nullptr, 0, nullptr, nullptr);
        std::string utf8_str(utf8_size, 0);
        WideCharToMultiByte(CP_UTF8, 0, utf16_str, utf16_len, &utf8_str[0], utf8_size, nullptr, nullptr);

        return utf8_str;
    }

    if (encoding == "Shift-JIS") {
        const char* shift_jis_str = buffer.data();
        int shift_jis_len = buffer.size();

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

export using MessageHandler = std::function<void(std::string message)>;

export class Engine
{
  protected:
    const char *name_;
    std::vector<Hook> hooks_;
    MessageHandler control_out_;
    MessageHandler lines_out_;

  public:
    Engine(const char *name, MessageHandler&& control_out, MessageHandler&& lines_out)
        : name_{name}, control_out_{std::move(control_out)}, lines_out_{std::move(lines_out)}
    {
    }

    virtual ~Engine()
    {
        std::cout << "Detaching ----" << name_ << std::endl;
        for (auto &hook : hooks_)
        {
            hook.Detach();
        }
    }

    void StartHookWatcher() {
        std::thread([this] {
          while (true) {
              std::this_thread::sleep_for(std::chrono::milliseconds(10));
              for (auto& hook : hooks_) {
                  hook.FlushReadyBuffers([this](const auto& buffer, const auto& encoding) {
                      std::string s = BufferToUTF8(buffer, encoding);
                      lines_out_(s);
                  });
              }
          }
        }).detach();
    }

    template <typename... Args>
    void ControlLog(std::format_string<Args...> fmt, Args&&... args) {
        auto s = std::format(fmt, args...);
        control_out_(s);
    }

    void AttachHooks()
    {
        ControlLog("Injecting ----");
        for (auto &hook : hooks_)
        {
            hook.Attach();
            std::cout << hook.GetName() << "attached" << std::endl;
        }
    }

};
