#include <gtest/gtest.h>
#include <string>

#include "dialogue.h"

class MockOutput : public INeoOutput
{
public:
    void outputDialogue(const std::string &id, const std::string &text) override
    {
        std::cout << id << ": " << text << std::endl;
    }
};

auto mock_output = MockOutput();

void SimulatePusher()
{
    auto DialogueA1 = "1111111111111111111111111111111111111111111111";
    auto DialogueA2 = "2222222222222222222222222222222222222222222222";

    auto DialogueB1 = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    auto DialogueB2 = "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";

    size_t sentence_length = 46;
    DialoguePool pool(mock_output, 1000);
    pool.Start();

    for (size_t i = 0; i < sentence_length; i++)
    {
        pool.PushTextToDialogue("DialogueA", "UTF-8", DialogueA1[i]);
        pool.PushTextToDialogue("DialogueB", "UTF-8", DialogueB1[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    for (size_t i = 0; i < 46; i++)
    {
        pool.PushTextToDialogue("DialogueA", "UTF-8", DialogueA2[i]);
        pool.PushTextToDialogue("DialogueB", "UTF-8", DialogueB2[i]);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

// Testing string おはよ
const unsigned char utf8_bytes[] = {0xE3, 0x81, 0x8A, 0xE3, 0x81, 0xAF, 0xE3, 0x82, 0x88};
std::string utf8_str(reinterpret_cast<const char *>(utf8_bytes), sizeof(utf8_bytes));
const unsigned char utf16le_bytes[] = {0x4a, 0x30, 0x6f, 0x30, 0x88, 0x30};
std::string utf16le_str(reinterpret_cast<const char *>(utf16le_bytes), sizeof(utf16le_bytes));
const unsigned char utf16be_bytes[] = {0x30, 0x4a, 0x30, 0x6f, 0x30, 0x88};
std::string utf16be_str(reinterpret_cast<const char *>(utf16be_bytes), sizeof(utf16be_bytes));
const unsigned char shift_jis_bytes[] = {0x82, 0xa8, 0x82, 0xcd, 0x82, 0xe6};
std::string shift_jis_str(reinterpret_cast<const char *>(shift_jis_bytes), sizeof(shift_jis_bytes));

TEST(Dialogue, UTF16LEDialogue)
{
    MockOutput output;
    auto dialogue_utf16le = Dialogue("UTF16Dialogue", "UTF-16LE", output);

    for (size_t i = 0; i < sizeof(utf16le_bytes); i++)
    {
        dialogue_utf16le.PushText(utf16le_bytes[i]);
    }

    auto actual_result = dialogue_utf16le.GetUTF8Text();

    EXPECT_EQ(utf8_str, actual_result);
}

TEST(Dialogue, ShiftJISDialogue)
{
    auto dialogue_shift_jis = Dialogue("ShiftJISDialogue", "Shift-JIS", mock_output);

    for (size_t i = 0; i < sizeof(shift_jis_bytes); i++)
    {
        dialogue_shift_jis.PushText(shift_jis_bytes[i]);
    }

    auto actual_result = dialogue_shift_jis.GetUTF8Text();

    EXPECT_EQ(utf8_str, actual_result);
}

TEST(Dialogue, PushAndFlush)
{
    SimulatePusher();
}