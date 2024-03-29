#pragma once

#include <MinHook.h>
#include <Windows.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <bit>
#include <cassert>
#include <concrt.h>
#include <cstdint>
#include <deque>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <ranges>
#include <regex>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef _WIN64
constexpr bool x64 = true;
#else
constexpr bool x64 = false;
#endif

template <typename T, typename... Xs> struct ArrayImpl
{
    using Type = std::tuple<T, Xs...>[];
};
template <typename T> struct ArrayImpl<T>
{
    using Type = T[];
};
template <typename... Ts> using Array = typename ArrayImpl<Ts...>::Type;

template <auto F>
using Functor = std::integral_constant<std::remove_reference_t<decltype(F)>,
                                       F>; // shouldn't need remove_reference_t but MSVC is bugged

struct PermissivePointer
{
    template <typename T> operator T *()
    {
        return (T *)p;
    }
    void *p;
};

template <typename HandleCloser = Functor<CloseHandle>> class AutoHandle
{
  public:
    AutoHandle(HANDLE h) : h(h)
    {
    }
    operator HANDLE()
    {
        return h.get();
    }
    PHANDLE operator&()
    {
        static_assert(sizeof(*this) == sizeof(HANDLE));
        assert(!h);
        return (PHANDLE)this;
    }
    operator bool()
    {
        return h.get() != NULL && h.get() != INVALID_HANDLE_VALUE;
    }

  private:
    struct HandleCleaner
    {
        void operator()(void *h)
        {
            if (h != INVALID_HANDLE_VALUE)
                HandleCloser()(PermissivePointer{h});
        }
    };
    std::unique_ptr<void, HandleCleaner> h;
};

template <typename T, typename M = std::mutex> class Synchronized
{
  public:
    template <typename... Args> Synchronized(Args &&...args) : contents(std::forward<Args>(args)...)
    {
    }

    struct Locker
    {
        T *operator->()
        {
            return &contents;
        }
        std::unique_lock<M> lock;
        T &contents;
    };

    Locker Acquire()
    {
        return {std::unique_lock(m), contents};
    }
    Locker operator->()
    {
        return Acquire();
    }
    T Copy()
    {
        return Acquire().contents;
    }

  private:
    T contents;
    M m;
};

template <typename F> void SpawnThread(const F &f) // works in DllMain unlike std thread
{
    F *copy = new F(f);
    CloseHandle(CreateThread(
        nullptr, 0,
        [](void *copy) {
            (*(F *)copy)();
            delete (F *)copy;
            return 0UL;
        },
        copy, 0, nullptr));
}

inline struct
{
    inline static BYTE DUMMY[100];
    template <typename T> operator T *()
    {
        static_assert(sizeof(T) < sizeof(DUMMY));
        return (T *)DUMMY;
    }
} DUMMY;

inline auto Swallow = [](auto &&...) {};

template <typename T> std::optional<std::remove_cv_t<T>> Copy(T *ptr)
{
    if (ptr)
        return *ptr;
    return {};
}

template <typename T> inline auto FormatArg(T arg)
{
    return arg;
}
template <typename C> inline auto FormatArg(const std::basic_string<C> &arg)
{
    return arg.c_str();
}

#pragma warning(push)
#pragma warning(disable : 4996)
template <typename... Args> inline std::string FormatString(const char *format, const Args &...args)
{
    std::string buffer(snprintf(nullptr, 0, format, FormatArg(args)...), '\0');
    sprintf(buffer.data(), format, FormatArg(args)...);
    return buffer;
}

template <typename... Args> inline std::wstring FormatString(const wchar_t *format, const Args &...args)
{
    std::wstring buffer(_snwprintf(nullptr, 0, format, FormatArg(args)...), L'\0');
    _swprintf(buffer.data(), format, FormatArg(args)...);
    return buffer;
}
#pragma warning(pop)

inline void Trim(std::wstring &text)
{
    text.erase(text.begin(), std::find_if_not(text.begin(), text.end(), iswspace));
    text.erase(std::find_if_not(text.rbegin(), text.rend(), iswspace).base(), text.end());
}

inline std::optional<std::wstring> StringToWideString(const std::string &text, UINT encoding)
{
    std::vector<wchar_t> buffer(text.size() + 1);
    if (int length = MultiByteToWideChar(encoding, 0, text.c_str(), text.size() + 1, buffer.data(), buffer.size()))
        return std::wstring(buffer.data(), length - 1);
    return {};
}

inline std::wstring StringToWideString(const std::string &text)
{
    std::vector<wchar_t> buffer(text.size() + 1);
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, buffer.data(), buffer.size());
    return buffer.data();
}

inline std::string WideStringToString(const std::wstring &text)
{
    std::vector<char> buffer((text.size() + 1) * 4);
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), -1, buffer.data(), buffer.size(), nullptr, nullptr);
    return buffer.data();
}

template <typename... Args> inline void TEXTRACTOR_MESSAGE(const wchar_t *format, const Args &...args)
{
    MessageBoxW(NULL, FormatString(format, args...).c_str(), L"Textractor", MB_OK);
}

template <typename... Args> inline void TEXTRACTOR_DEBUG(const wchar_t *format, const Args &...args)
{
    SpawnThread([=] { TEXTRACTOR_MESSAGE(format, args...); });
}

void Localize();

#ifdef _DEBUG
#define TEST(...)                                                                                                      \
    static auto _ = CreateThread(                                                                                      \
        nullptr, 0,                                                                                                    \
        [](auto) {                                                                                                     \
            __VA_ARGS__;                                                                                               \
            return 0UL;                                                                                                \
        },                                                                                                             \
        NULL, 0, nullptr)
#else
#define TEST(...)
#endif


/*
 *  address_t
 *  A simple wrapper around uintptr_t to allow for easy manipulation of memory addresses.
 */
class address_t {
private:
    uintptr_t address;

public:
    constexpr address_t() : address(0) {}

    constexpr address_t(uintptr_t addr) : address(addr) {}

    constexpr address_t(void* ptr) : address(std::bit_cast<uintptr_t>(ptr)) {}

    constexpr address_t& operator=(void* ptr) {
        address = std::bit_cast<uintptr_t>(ptr);
        return *this;
    }

    constexpr address_t& operator=(uintptr_t addr) {
        address = addr;
        return *this;
    }

    constexpr address_t operator+(intptr_t offset) const {
        return address_t(address + offset);
    }

    constexpr address_t operator+(uintptr_t offset) const {
        return address_t(address + offset);
    }

    constexpr address_t operator-(intptr_t offset) const {
        return address_t(address - offset);
    }

    constexpr address_t& operator+=(intptr_t offset) {
        address += offset;
        return *this;
    }

    constexpr address_t& operator-=(intptr_t offset) {
        address -= offset;
        return *this;
    }

    constexpr address_t& operator>>=(uintptr_t digits) {
        address >>= digits;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const address_t& addr) {
        os << std::uppercase << std::hex << std::setfill('0') << std::setw(sizeof(std::uintptr_t) * 2) << addr.address << std::dec;
        return os;
    }

    constexpr operator void*() const {
        return std::bit_cast<void*>(address);
    }

    constexpr operator uintptr_t() const {
        return address;
    }

    constexpr operator address_t*() const {
        return std::bit_cast<address_t*>(address);
    }
};