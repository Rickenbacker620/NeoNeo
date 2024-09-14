#pragma once

#include <MinHook.h>
#include <Windows.h>
#include <bit>
#include <iostream>
#include <vector>
#include <iomanip>

template <typename T>
concept PointerOrUintptr = std::is_pointer_v<T> || std::is_same_v<T, uintptr_t>;

template <typename T>
concept Integral = std::is_integral_v<T>;

/*
 *  address_t
 *  A simple wrapper around uintptr_t to allow for easy manipulation of memory addresses.
 */
class address_t {
private:
    uintptr_t address;

public:
    constexpr address_t() : address(0) {}

    // Generalized constructor using the concept
    template <PointerOrUintptr T>
    constexpr address_t(T addr) : address(std::bit_cast<uintptr_t>(addr)) {}

    // Generalized assignment operator using the concept
    template <PointerOrUintptr T>
    constexpr address_t& operator=(T addr) {
        address = std::bit_cast<uintptr_t>(addr);
        return *this;
    }
    // Template-based operator+ that works with any integral type
    template <Integral T>
    constexpr address_t operator+(T offset) const {
        return {address + static_cast<uintptr_t>(offset)};
    }

    // Handle the case where the other operand is an address_t
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

    //use spaceship operator instead
    constexpr auto operator<=>(const address_t& other) const {
        return address <=> other.address;
    }

    constexpr bool operator! () const {
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

    // Templated conversion operator for casting to any pointer type or uintptr_t
    template <PointerOrUintptr T>
    constexpr operator T() const {
        return std::bit_cast<T>(address);
    }
};