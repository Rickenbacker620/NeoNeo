module;
#include <Windows.h>
export module engine_base;

import <vector>;
import <iostream>;
import <functional>;
import <thread>;
import <utility>;

import hook;
import console;

std::string BufferToUTF8(const std::vector<char>& buffer, const std::string& encoding) {

    if (encoding == "UTF-8") {
        return {buffer.begin(), buffer.end()};
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

export class Engine
{
  protected:
    const char *name_;
    std::vector<Hook> hooks_;

  public:
    explicit Engine(const char* name) : name_{name}
    {
    }

    virtual ~Engine()
    {
        ConsoleOut::log("Engine shutting down");
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
                  hook.FlushReadyBuffers([this](const std::string& hookName, const auto& buffer, const auto& encoding) {
                      std::string s = BufferToUTF8(buffer, encoding);
                      LinesOut::log(hookName, s);
                  });
              }
          }
        }).detach();
    }

    template <typename... Args>
    void ControlLog(std::format_string<Args...> fmt, Args&&... args) {
        auto s = std::format(fmt, std::forward<Args>(args)...);
        ConsoleOut::log(s);
    }

    void AttachHooks()
    {
        ControlLog("[Hook] Start Attaching...");
        for (auto &hook : hooks_)
        {
            hook.Attach();
            ControlLog("[Hook] Attached: {}", hook.GetName());
        }
        ControlLog("[Hook] Finished Attaching");
    }

};
