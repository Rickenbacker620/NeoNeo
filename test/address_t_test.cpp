#include <gtest/gtest.h>
#include <iostream>
#include <bit>
#include <iomanip>
#include <compare>
#include "common.h"

// Assuming the 'address_t' class is already defined as shown earlier

// Test fixture for address_t tests
class AddressTest : public ::testing::Test {
protected:
    void SetUp() override {
        // This will run before each test.
        // We can use it for shared setup if necessary.
    }

    void TearDown() override {
        // This will run after each test.
    }
};

// Test case for default constructor
TEST_F(AddressTest, DefaultConstructor) {
    address_t addr;
    ASSERT_EQ(static_cast<uintptr_t>(addr), 0);
}

// Test case for constructing from uintptr_t
TEST_F(AddressTest, ConstructFromUintptr) {
    uintptr_t testAddr = 0x1234;
    address_t addr(testAddr);
    ASSERT_EQ(static_cast<uintptr_t>(addr), testAddr);
}

// Test case for constructing from a void pointer
TEST_F(AddressTest, ConstructFromVoidPtr) {
    int var = 42;
    address_t addr(&var);
    ASSERT_EQ(static_cast<void*>(addr), &var);
}

// Test case for constructing from different pointer types
TEST_F(AddressTest, ConstructFromCharPtr) {
    char c = 'a';
    address_t addr(&c);
    ASSERT_EQ(static_cast<char*>(addr), &c);
}

// Test case for assignment from void pointer
TEST_F(AddressTest, AssignmentVoidPtr) {
    int var = 100;
    address_t addr;
    addr = &var;
    ASSERT_EQ(static_cast<void*>(addr), &var);
}

// Test case for assignment from uintptr_t
TEST_F(AddressTest, AssignmentUintptr) {
    uintptr_t testAddr = 0xABCDEF;
    address_t addr;
    addr = testAddr;
    ASSERT_EQ(static_cast<uintptr_t>(addr), testAddr);
}

// Test case for arithmetic addition
TEST_F(AddressTest, ArithmeticAdd) {
    uintptr_t testAddr = 0x1000;
    address_t addr(testAddr);

    address_t addrPlus10 = addr + 10;
    ASSERT_EQ(static_cast<uintptr_t>(addrPlus10), testAddr + 10);
}

// Test case for arithmetic subtraction
TEST_F(AddressTest, ArithmeticSubtract) {
    uintptr_t testAddr = 0x2000;
    address_t addr(testAddr);

    auto addrMinus20 = addr - 20;
    ASSERT_EQ(static_cast<uintptr_t>(addrMinus20), testAddr - 20);
}

// Test case for addition assignment operator
TEST_F(AddressTest, AdditionAssignment) {
    uintptr_t testAddr = 0x3000;
    address_t addr(testAddr);

    addr += 0x10;
    ASSERT_EQ(addr, testAddr + 0x10);
}

// Test case for subtraction assignment operator
TEST_F(AddressTest, SubtractionAssignment) {
    uintptr_t testAddr = 0x4000;
    address_t addr(testAddr);

    addr -= 0x20;
    ASSERT_EQ(addr, testAddr - 0x20);
}

// Test case for shift right assignment
TEST_F(AddressTest, ShiftRightAssignment) {
    uintptr_t testAddr = 0x8000;
    address_t addr(testAddr);

    addr >>= 2;
    ASSERT_EQ(uintptr_t(addr), testAddr >> 2);
}

// Test case for comparison operators (spaceship operator)
TEST_F(AddressTest, SpaceshipOperator) {
    address_t addr1 = uintptr_t(0x1000);
    address_t addr2 = uintptr_t(0x2000);
    address_t addr3 = uintptr_t(0x1000);

    ASSERT_TRUE(addr1 < addr2);
    ASSERT_TRUE(addr2 > addr1);
    ASSERT_TRUE(addr1 == addr3);
}

// Test case for conversion to uintptr_t
TEST_F(AddressTest, ConversionToUintptr) {
    uintptr_t testAddr = 0xDEADBEEF;
    address_t addr(testAddr);

    uintptr_t asUintptr = static_cast<uintptr_t>(addr);
    ASSERT_EQ(asUintptr, testAddr);
}

// Test case for conversion to void pointer
TEST_F(AddressTest, ConversionToVoidPtr) {
    int var = 42;
    address_t addr(&var);

    void* asVoidPtr = static_cast<void*>(addr);
    ASSERT_EQ(asVoidPtr, &var);
}

// Test case for conversion to char pointer
TEST_F(AddressTest, ConversionToCharPtr) {
    char c = 'A';
    address_t addr(&c);

    char* asCharPtr = static_cast<char*>(addr);
    ASSERT_EQ(asCharPtr, &c);
}