module;

#include <Windows.h>

export module common;

import <bit>;
import <iostream>;
import <vector>;
import <iomanip>;
import <concepts>;
import <compare>;

export template <typename T>
concept PointerOrUintptr = std::is_pointer_v<T> || std::is_same_v<T, uintptr_t>;

export template <typename T>
concept Integral = std::is_integral_v<T>;

export class address_t {
  private:
    uintptr_t address;

  public:
    constexpr address_t() : address(0) {}

    template <PointerOrUintptr T>
    constexpr address_t(T addr) : address(std::bit_cast<uintptr_t>(addr)) {}

    template <PointerOrUintptr T>
    constexpr address_t& operator=(T addr) {
        address = std::bit_cast<uintptr_t>(addr);
        return *this;
    }

    template <Integral T>
    constexpr address_t operator+(T offset) const {
        return {address + static_cast<uintptr_t>(offset)};
    }

    constexpr address_t operator+(const address_t& other) const {
        return {address + other.address};
    }

    template <Integral T>
    constexpr address_t operator-(T offset) const {
        return {address - static_cast<uintptr_t>(offset)};
    }

    constexpr address_t& operator+=(intptr_t offset) {
        address += offset;
        return *this;
    }

    constexpr address_t& operator-=(intptr_t offset) {
        address -= offset;
        return *this;
    }

    constexpr bool operator==(const address_t& other) const {
        return address == other.address;
    }

    constexpr auto operator<=>(const address_t& other) const {
        return address <=> other.address;
    }

    constexpr bool operator!() const {
        return !address;
    }

    constexpr address_t& operator>>=(uintptr_t digits) {
        address >>= digits;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const address_t& addr) {
        os << std::uppercase << std::hex << std::setfill('0') << std::setw(sizeof(std::uintptr_t) * 2) << addr.address << std::dec;
        return os;
    }

    char GetAsByte() const {
        return *reinterpret_cast<char*>(address);
    }

    address_t GetAsAddress() const {
        return *reinterpret_cast<address_t*>(address);
    }


    template <PointerOrUintptr T>
    constexpr operator T() const {
        return std::bit_cast<T>(address);
    }
};

struct Dummy{
    BYTE DUMMY[100];
    template <typename T> operator T *() {
        static_assert(sizeof(T) < sizeof(DUMMY));
        return (T *)DUMMY;
    }
};

export Dummy DUMMY;
