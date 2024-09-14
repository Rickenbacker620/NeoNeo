#include <gtest/gtest.h>
#include "server.h"
#include <thread>
#include <chrono>

TEST(ServerTest, Start) {
    NeoServer server;
    for (size_t i = 0; i < 10; i++) {
        server.outputDialogue("DialogueA", "Hello, World!");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}