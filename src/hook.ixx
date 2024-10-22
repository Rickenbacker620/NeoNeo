module;

#include <MinHook.h>

export module hook;

import <cstring>;
import <string>;
import <vector>;
import <optional>;
import <unordered_map>;
import <utility>;
import <chrono>;

import dialogue;
import common;
import hook_param;

auto addr_hash = [](const address_t &addr) { return std::hash<uintptr_t>{}(addr); };

export class Hook
{
    using TimeStamp = std::chrono::time_point<std::chrono::steady_clock>;
    using TimeStampBuffer = std::pair<TimeStamp, std::vector<char>>;
  private:
    HookParam param_{};
    BYTE trampoline[40]{};
    std::chrono::milliseconds flush_timeout_;
    std::unordered_map<address_t, TimeStampBuffer, decltype(addr_hash)> buffer_{};

    size_t GetTextLength(address_t base, address_t text_addr) const
    {
        auto &attribute_ = this->param_.attribute;
        auto &text_offset_ = this->param_.text_offset;

        auto GetNullTerminatedLength = [&](address_t addr) -> size_t {
            return attribute_ & USING_UTF16 ? std::wcslen(std::bit_cast<const wchar_t *>(addr)) * 2
                                            : std::strlen(std::bit_cast<const char *>(addr));
        };

        if (attribute_ & USING_STRING)
        {
            if (text_offset_.length.has_value())
            {
                size_t length = *(static_cast<int *>(base + text_offset_.length.value()));
                if (length == -1)
                {
                    return GetNullTerminatedLength(text_addr);
                }
                else
                {
                    return attribute_ & USING_UTF16 ? length * 2 : length;
                }
            }
            else
            {
                return GetNullTerminatedLength(text_addr);
            }
        }
        else
        {
            if (attribute_ & USING_UTF16)
            {
                return 2;
            }
            else
            {
                if (attribute_ & BIG_ENDIAN)
                {
                    text_addr >>= 8;
                }
                return 1;
            }
        }
    }

    address_t GetTextAddress(address_t base) const
    {
        auto &text_offset_ = this->param_.text_offset;
        address_t address = base + text_offset_.data.first;
        if (text_offset_.data.second.has_value())
        {
            address = address.GetAsAddress() + text_offset_.data.second.value();
        }
        return address;
    }

    address_t GetTextContext(address_t base) const
    {
        auto &text_offset_ = this->param_.text_offset;
        address_t context = base.GetAsAddress();
        if (text_offset_.context.has_value())
        {
            context = (context.GetAsAddress() + text_offset_.context.value().first).GetAsAddress();
        }
        return context;
    }

  public:
    explicit Hook(HookParam param) : param_{param}
    {
    }

    std::string GetName() const
    {
        auto &address_ = this->param_.address;
        std::string name = std::format("{:p}", (void *)address_.offset);
        if (!address_.module.empty())
        {
            name += std::format(":{:s}", address_.module);
        }
        if (!address_.function.empty())
        {
            name += std::format("::{:s}", address_.function);
        }
        return name;
    }

//    void NeedFlush()
//    {
//        for (auto &[context, buffer] : buffer_)
//        {
//            std::string encoding = param_.attribute & USING_UTF8 ? "UTF-8" : param_.attribute & USING_UTF16 ? "UTF-16LE" : "Shift-JIS";
//            std::string dialogue_id = std::format("{:s}\x02{:d}\x02{:p}\x02{:p}\x02{:p}\x02{:s}", this->GetName(), 1234,
//                                                  (void *)param_.address.GetAddress(), (void *)context.GetAddress(), (void *)0, encoding);
//            Sink::Flush(dialogue_id, encoding, buffer);
//        }
//        buffer_.clear();
//    }

    void FlushReadyBuffers() {
        // for (auto &[context, buffer] : buffer) {
        for (auto &[context, buffer]: buffer_) {

        }
    }


    void OnReceive(address_t base)
    {
        auto &attribute_ = this->param_.attribute;
        auto &text_offset_ = this->param_.text_offset;
        auto &address_ = this->param_.address;

        auto hook_addr = address_.GetAddress();
        auto text_address = GetTextAddress(base);

        if (!text_address)
        {
            return;
        }

        auto text_context = GetTextContext(base);
        auto text_length = GetTextLength(base, text_address);

        std::string encoding = attribute_ & USING_UTF8 ? "UTF-8" : attribute_ & USING_UTF16 ? "UTF-16LE" : "Shift-JIS";

        auto dialogue_id = std::format("{:s}\x02{:d}\x02{:p}\x02{:p}\x02{:p}\x02{:s}", this->GetName(), 1234,
                                       (void *)address_.GetAddress(), (void *)text_context, (void *)0, encoding);

        if (text_length == 2)
        {
            Sink::Push(dialogue_id, encoding, *(static_cast<char *>(text_address)));
            Sink::Push(dialogue_id, encoding, *(static_cast<char *>(text_address + 1)));
            if (buffer_.find(text_context) == buffer_.end())
            {
                buffer_[text_context] = TimeStampBuffer{std::chrono::steady_clock::now(), std::vector<char>(30)};
            }
            buffer_[text_context].first = std::chrono::steady_clock::now();
            buffer_[text_context].second.push_back(*(static_cast<char *>(text_address)));
            buffer_[text_context].second.push_back(*(static_cast<char *>(text_address + 1)));
        }
        else
        {
            char temp[1000];
            std::memcpy(temp, text_address, text_length);
            for (size_t i = 0; i < text_length; i++)
            {
                Sink::Push(dialogue_id, encoding, temp[i]);
            }
        }
    }

    bool Attach()
    {
        BYTE common_hook[] = {0x9c, 0x60, 0x9c, 0x8d, 0x44, 0x24, 0x28, 0x50, 0xb9, 0, 0, 0, 0, 0xbb, 0,
                              0,    0,    0,    0xff, 0xd3, 0x9d, 0x61, 0x9d, 0x68, 0, 0, 0, 0, 0xc3};
        int this_offset = 9, send_offset = 14, original_offset = 24;
        auto &address_ = this->param_.address;

        VirtualProtect((LPVOID)(0x75BE0410), 10, PAGE_EXECUTE_READWRITE, DUMMY);
        VirtualProtect((LPVOID)(&trampoline), 40, PAGE_EXECUTE_READWRITE, DUMMY);

        void *original;
        MH_STATUS error;
        auto address = address_.GetAddress();

        while ((error = MH_CreateHook(address, trampoline, &original)) != MH_OK)
        {
            if (error == MH_ERROR_ALREADY_CREATED)
            {
                MH_DisableHook(address);
                MH_RemoveHook(address);
                std::cout << "hook conflict, removing existing hook" << std::endl;
            }
            else
            {
                std::cout << MH_StatusToString(error) << address << std::endl << this->trampoline << std::endl;
            }
        }

        using HookThisType = Hook *;
        using HookSendType = void (Hook:: *)(address_t);
        *reinterpret_cast<HookThisType *>(common_hook + this_offset) = this;
        *reinterpret_cast<HookSendType *>(common_hook + send_offset) = &Hook::OnReceive;

        *(void **)(common_hook + original_offset) = original;
        memcpy(trampoline, common_hook, sizeof(common_hook));
        return MH_EnableHook(address) == MH_OK;
    }

    void Detach()
    {
        auto &address_ = this->param_.address;
        auto address = address_.GetAddress();
        MH_DisableHook(address);
        MH_RemoveHook(address);
    }
};
